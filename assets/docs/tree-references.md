**Stacking Context and Layer Reference Sheet**

---

## 1️⃣ Stacking Context

**Purpose:**

* Logical grouping for painting order and compositing.
* Created by elements with properties like `opacity < 1`, `transform`, `z-index` (positioned), `filter`, etc.

**Structure:**

```cpp
struct StackingContext {
    Element* owner;               // Element that created this SC
    std::vector<Element*> normalFlow; // Elements painted inside this SC
    std::vector<std::unique_ptr<StackingContext>> children; // Nested SCs
};

struct Element {
    std::vector<std::unique_ptr<Element>> children;
    std::weak_ptr<StackingContext> stackingContext; // back-pointer to owning SC
};
```

**BFS Build Example:**

```cpp
std::unique_ptr<StackingContext> buildStackingContextTreeBFS(Element* root) {
    auto rootSC = std::make_unique<StackingContext>();
    rootSC->owner = root;

    std::queue<std::pair<Element*, StackingContext*>> q;
    q.push({root, rootSC.get()});

    while (!q.empty()) {
        auto [elem, parentSC] = q.front(); q.pop();

        for (auto& childPtr : elem->children) {
            Element* child = childPtr.get();
            if (createsStackingContext(child)) {
                auto childSC = std::make_unique<StackingContext>();
                childSC->owner = child;
                child->stackingContext = childSC;

                parentSC->children.push_back(std::move(childSC));
                q.push({child, parentSC->children.back().get()});
            } else {
                parentSC->normalFlow.push_back(child);
                child->stackingContext = parentSC;
                q.push({child, parentSC});
            }
        }
    }
    return rootSC;
}
```

**Example Tree:**

```
RootSC
 └─ SC1 (div, opacity 0.8)
      ├─ normalFlow = [div Text]
      └─ children = [SC2 (div transform)]
           └─ normalFlow = [span Inner]
```

**Rendering Steps / Process (Stacking Context):**

1. Paint `owner` element (background, borders, content).
2. Paint all `normalFlow` elements inside this SC.
3. Recursively paint child SCs based on z-index order.
4. Apply compositing rules (opacity, transforms) of the owner to the entire SC.

**Pseudocode:**

```cpp
void renderStackingContext(StackingContext* sc) {
    paintElement(sc->owner);
    for (auto* e : sc->normalFlow) paintElement(e);
    for (auto& childSC : sc->children) renderStackingContext(childSC.get());
    applyCompositing(sc->owner);
}
```

---

## 2️⃣ Layer

**Purpose:**

* Represents a GPU-backed compositing surface.
* Contains elements that paint into it and child layers.

**Structure:**

```cpp
struct Layer {
    Element* owner;  // Element that caused this layer
    std::vector<Element*> paintNodes; // Elements painted directly into this layer
    std::vector<std::unique_ptr<Layer>> children; // Child layers
};
```

**Build Example:**

```cpp
std::unique_ptr<Layer> buildLayerTree(StackingContext* sc, Layer* parentLayer = nullptr) {
    std::unique_ptr<Layer> layer = nullptr;

    if (sc->owner->createsLayer) {
        // Create a dedicated GPU surface
        layer = std::make_unique<Layer>();
        layer->owner = sc->owner;
        sc->layer = layer.get(); // SC knows about its GPU layer
    } else {
        // No GPU layer, paint into parent's layer
        sc->layer = parentLayer;
    }

    // Add elements to this layer’s paint list
    for (Element* e : sc->normalFlow) {
        if (sc->layer) {
            sc->layer->paintNodes.push_back(e);
        }
    }

    // Recurse into children SC
    for (auto& childSC : sc->children) {
        auto childLayer = buildLayerTree(childSC.get(), sc->layer);
        if (childLayer) {
            sc->layer->children.push_back(std::move(childLayer));
        }
    }

    return layer;
}

```

**Example Tree:**

```
RootLayer
 └─ Layer(owner=div, opacity=0.8)
      paintNodes = [p]
      children = [
          Layer(owner=div transform)
              paintNodes = [span]
              children = []
      ]
```

**Rendering Steps / Process (Layer):**

1. Paint `owner` element into this layer's surface.
2. Paint all `paintNodes` elements into the same surface.
3. Render child layers into their own surfaces.
4. Composite child layers into parent layer with correct transforms and opacity.
5. Composite parent layer into root layer or screen.

**Pseudocode:**

```cpp
void renderLayer(Layer* layer) {
    paintElement(layer->owner);
    for (auto* e : layer->paintNodes) paintElement(e);
    for (auto& child : layer->children) renderLayer(child.get());
    compositeLayer(layer);
}
```

---

**Summary:**

| Concept         | Owner                        | Normal / Paint          | Children     |
| --------------- | ---------------------------- | ----------------------- | ------------ |
| StackingContext | Element that triggered SC    | Elements in normal flow | Nested SCs   |
| Layer           | Element that triggered layer | Elements painted inside | Child layers |

## compositeLayer
1. Take all child layers (Layer::children).
2. Draw their RenderTexture2D onto the parent layer's texture.
3. Apply transformations (position, rotation, scale) if the child layer has any.
4. Apply opacity/blending of both the child layer and the parent layer (if needed).
5. Optionally handle clipping/masks.
```C++
void compositeLayer(Layer* layer) {
    // Begin drawing on the parent layer's texture
    BeginTextureMode(layer->renderTexture);

    for (auto& child : layer->children) {
        // Assume child has a transform & opacity stored
        Rectangle srcRec = {0, 0, (float)child->renderTexture.texture.width, -(float)child->renderTexture.texture.height};
        Vector2 dstPos = {child->position.x, child->position.y};
        float rotation = child->rotation;    // degrees
        Vector2 origin = {0, 0};
        Color tint = {255, 255, 255, (unsigned char)(255 * child->opacity)}; // apply opacity

        // Draw the child layer's texture onto the parent layer
        DrawTexturePro(child->renderTexture.texture, srcRec, 
                       {dstPos.x, dstPos.y, srcRec.width, srcRec.height}, origin, rotation, tint);
    }

    EndTextureMode();
}
```

## actual rendering
Rough build blocks
```C++
struct Element {
    std::vector<std::unique_ptr<Element>> children;
    std::weak_ptr<StackingContext> stackingContext;
    // Element properties
    bool createsSC = false;
    bool createsLayer = false;
};

struct StackingContext {
    Element* owner;
    std::vector<Element*> normalFlow;
    std::vector<std::unique_ptr<StackingContext>> children;
    Layer* layer = nullptr; // optional layer if GPU surface is created
};

struct Layer {
    Element* owner;
    std::vector<Element*> paintNodes;
    std::vector<std::unique_ptr<Layer>> children;
    RenderTexture2D renderTexture;
};

```
__Assumptions__
1. Each element can optionally create a stacking context.
2. Each stacking context can create a GPU layer if needed.
3. Painting respects:
    - owner → element that created SC/layer
    - normalFlow → elements inside SC
    - children → child SCs / layers

```C++
void renderStackingContextFrame(StackingContext* sc) {
    // 1️⃣ Prepare layer if exists
    if (sc->layer) {
        BeginTextureMode(sc->layer->renderTexture);
        ClearBackground(BLANK);

        // 2️⃣ Paint owner and normal flow
        paintElement(sc->owner);
        for (Element* e : sc->normalFlow) paintElement(e);

        EndTextureMode();
    }

    // 3️⃣ Recursively render children SCs
    for (auto& childSC : sc->children) {
        renderStackingContextFrame(childSC.get());
    }

    // 4️⃣ Composite child layers if this SC has a layer
    if (sc->layer) {
        BeginTextureMode(sc->layer->renderTexture);
        for (auto& childSC : sc->children) {
            if (childSC->layer)
                compositeLayer(childSC->layer);
        }
        EndTextureMode();
    }
}
```