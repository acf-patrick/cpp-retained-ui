#include <algorithm>
#include <memory>
#include <queue>
#include <raylib.h>
#include <repository.h>
#include <stack>
#include <ui.h>
#include <unordered_set>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

/*
def hitTest(rootCtx, point):
  stack: Stack[(StackingContext, Bool)] = [(rootCtx, false)]

  while !stack.empty():
    ctx, visited = stack.pop()
    if !visited:
      stack.push((ctx, true))
    else:
      for e in ctx.elements:
        if e.contains(point):
          return e
      for child in ctx.children.reversed():
        stack.push(child)
  return null

def renderCtxTree(rootCtx: StackingContext):
  stack: Stack[StackingContext] = [rootCtx]
  clearedLayers: Set[LayerId] = {}

  while !stack.empty():
    ctx = stack.pop()
    renderCtx(ctx, clearedLayers)
#    ctx.children.sort(sortCtxByReverseZIndex)
    for child in ctx.children.reversed():
      stack.push(child)

def renderCtx(ctx, clearedLayers):
  layer = ctx.layer
  layer.lock()
  if !clearedLayers.contains(layer.id):
    layer.clear()
    clearedLayers.add(layer.id)
  ctx.owner.render()
#  ctx.elements.sort(sortElementsByZIndex)
  for e in ctx.elements:
    e.render()
  layer.release()

def compositeLayerTree(rootLayer: Layer):
  stack: Stack[(Layer, Bool)] = [(rootLayer, false)]
  while !stack.empty():
    layer, visited = stack.pop()
    if !visited:
      stack.push((layer, true))
    else:
      if layer.parent:
        layer.parent.lock()
        layer.render() # Apply owner properties then render texture
        layer.parent.release()

      for child in layer.children:
        stack.push((child, false))

def renderFrame(rootCtx, rootLayer):
  renderCtxTree(rootCtx)
  compositeLayerTree(rootLayer)
  BeginDrawing()
  rootLayer.render() # Apply owner properties then render texture

  EndDrawing()
*/

class Engine {
    struct WindowInitialization {
        WindowInitialization() {
            InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Retained UI with raylib");
        }

        ~WindowInitialization() {
            CloseWindow();
        }
    };

    WindowInitialization _windowInit; // gets destroyed last
    std::shared_ptr<ui::element::Root> _elementsRoot;
    std::shared_ptr<ui::rendering::StackingContext> _stackingContextRoot;
    std::shared_ptr<ui::rendering::Layer> _layerRoot;
    std::vector<repository::Repository *> _repositories;

    std::shared_ptr<ui::rendering::StackingContext> buildStackingContextTree(std::shared_ptr<ui::element::Element> elementsRoot) const {
        // [currentElement, parentCtx]
        std::queue<std::pair<std::shared_ptr<ui::element::Element>, std::shared_ptr<ui::rendering::StackingContext>>> queue;
        std::shared_ptr<ui::rendering::StackingContext> rootCtx;

        queue.push({elementsRoot, nullptr});
        while (!queue.empty()) {
            auto [e, parentCtx] = queue.front();
            queue.pop();

            if (ui::rendering::StackingContext::IsRequiredFor(e)) {
                auto ctx = std::make_shared<ui::rendering::StackingContext>(e);
                e->setStackingContext(ctx);
                if (parentCtx)
                    parentCtx->appendChild(ctx);

                if (e == elementsRoot)
                    rootCtx = ctx;
            } else {
                parentCtx->addElement(e);
                e->setStackingContext(parentCtx);
            }

            for (auto child : e->getChildren())
                queue.push({child, e->getStackingContext()});
        }

        return rootCtx;
    }

    std::shared_ptr<ui::rendering::Layer> buildLayerTree(std::shared_ptr<ui::rendering::StackingContext> rootCtx) const {
        // [currentCtx, parentLayer]
        std::queue<std::pair<std::shared_ptr<ui::rendering::StackingContext>, std::shared_ptr<ui::rendering::Layer>>> queue;
        std::shared_ptr<ui::rendering::Layer> rootLayer;

        queue.push({rootCtx, nullptr});
        while (!queue.empty()) {
            auto [ctx, parentLayer] = queue.front();
            queue.pop();
            auto ctxOwner = ctx->getOwner();

            if (ui::rendering::Layer::IsRequiredFor(ctxOwner)) {
                auto layer = std::make_shared<ui::rendering::Layer>(ctxOwner);
                ctx->setLayer(layer);
                if (parentLayer)
                    parentLayer->appendChild(layer);

                if (ctx == rootCtx)
                    rootLayer = layer;
            } else {
                ctx->setLayer(parentLayer);
            }

            for (auto child : ctx->getChildren())
                queue.push({child, ctx->getLayer()});
        }

        return rootLayer;
    }

    void scaffold() {
        _elementsRoot = std::make_shared<ui::element::Root>(Vector2{
            .x = WINDOW_WIDTH,
            .y = WINDOW_HEIGHT});

        auto view = std::make_shared<ui::element::View>();
        _elementsRoot->appendChild(view);

        {
            auto layout = view->getLayout();
            auto &flex = layout.flex.emplace();
            flex.justifyContent = ui::style::JustifyContent::Center;
            flex.alignItems = ui::style::Alignment::Center;
            flex.flex = 1.0;
            view->updateLayout(layout);
        }

        auto rect = std::make_shared<ui::element::View>();
        view->appendChild(rect);
        {
            auto layout = rect->getLayout();
            auto &size = layout.size.emplace();
            size.width = utils::Value(64);
            size.height = utils::Value(64);
            layout.positionType = ui::style::PositionType::Absolute;
            auto &position = layout.position.emplace();
            position.top = utils::Value(0.0f);
            position.right = utils::Value(0.0f);
            rect->updateLayout(layout);

            auto style = rect->getStyle();
            style.backgroundColor = BLUE;
            style.zIndex = 1;
            style.isolation = ui::style::IsolationIsolate{};
            rect->updateStyle(style);
        }

        auto button = std::make_shared<ui::element::Button>();
        // ui::element::Element::AppendChild(view, button);

        {
            auto style = button->getStyle();
            style.inheritables.fontSize = 24;
            style.inheritables.color = BROWN;
            std::vector<std::string> fontFamily;
            fontFamily.push_back("roboto");
            style.inheritables.fontFamily = fontFamily;
            button->updateStyle(style);
        }

        auto text = std::make_shared<ui::element::Text>("This is a button");
        button->appendChild(text);

        repository::FontRepository::Get()->load("roboto", "Roboto-Regular.ttf");

        auto imgContainer = std::make_shared<ui::element::View>();
        view->appendChild(imgContainer);
        {
            auto layout = imgContainer->getLayout();
            layout.spacing.emplace().border = 3;
            auto &size = layout.size.emplace();
            size.width = utils::Ratio(0.9);
            size.height = utils::Ratio(0.9);
            imgContainer->updateLayout(layout);

            auto style = imgContainer->getStyle();
            style.borderColor = WHITE;
            auto &transform = style.transform.emplace();
            // transform.rotation = {utils::AngleRadian(PI/3)};
            auto &translation = transform.translation.emplace();
            /*translation.x = utils::Value<float>(48);
            translation.y = utils::Value<float>(-48);
            auto& scale = transform.scale.emplace();
            scale.x = 0.75;
            scale.y = 0.75;*/

            imgContainer->updateStyle(style);
        }

        auto image = std::make_shared<ui::element::Image>("assets/images/cat.png", "cat");
        imgContainer->appendChild(image);
        {
            auto style = image->getStyle();
            style.opacity = 1;
            auto &props = *style.drawableContentProps;
            props.objectFit = ui::style::ObjectFit::ScaleDown;
            props.objectPosition = ui::style::ObjectPositionCenter{};
            image->updateStyle(style);

            auto layout = image->getLayout();
            auto &size = layout.size.emplace();
            size.width = utils::Ratio(1.0);
            size.height = utils::Ratio(1.0);

            image->updateLayout(layout);
        }

        _elementsRoot->finalize();
    }

    void renderElements() {
        std::queue<std::shared_ptr<ui::element::Element>> queue;

        queue.push(_elementsRoot);
        while (!queue.empty()) {
            auto e = queue.front();
            queue.pop();
            e->render();
            for (auto child : e->getChildren())
                queue.push(child);
        }
    }

    void compositeLayerTree(std::shared_ptr<ui::rendering::Layer> rootLayer) {
        std::stack<std::pair<std::shared_ptr<ui::rendering::Layer>, bool>> stack;
        stack.push({rootLayer, false});

        while (!stack.empty()) {
            auto [layer, visited] = stack.top();
            stack.pop();

            if (visited) {
                if (auto parent = layer->getParent()) {
                    auto useLayerGuard = parent->use();
                    layer->render();
                }
            } else {
                stack.push({layer, true});

                auto children = layer->getChildren();
                std::sort(children.begin(), children.end(),
                          [](std::shared_ptr<ui::rendering::Layer> layerA, std::shared_ptr<ui::rendering::Layer> layerB) {
                              return layerA->getContext().zIndex > layerB->getContext().zIndex;
                          });

                for (auto child : children)
                    stack.push({child, false});
            }
        }
    }

    void renderStackingContextTree(std::shared_ptr<ui::rendering::StackingContext> rootCtx) {
        std::stack<std::shared_ptr<ui::rendering::StackingContext>> stack;
        stack.push(rootCtx);

        while (!stack.empty()) {
            auto ctx = stack.top();
            stack.pop();

            ctx->render();

            auto children = ctx->getChildren();
            std::sort(children.begin(), children.end(),
                      [](std::shared_ptr<ui::rendering::StackingContext> ctxA, std::shared_ptr<ui::rendering::StackingContext> ctxB) {
                          return ctxA->getContext().zIndex > ctxB->getContext().zIndex;
                      });

            for (auto child : children)
                stack.push(child);
        }
    }

    void render() {
        renderStackingContextTree(_stackingContextRoot);
        compositeLayerTree(_layerRoot);
        _layerRoot->render();
    }

  public:
    Engine() {
        _repositories = repository::InitRepositories();
        scaffold();

        _stackingContextRoot = buildStackingContextTree(_elementsRoot);
        _layerRoot = buildLayerTree(_stackingContextRoot);

        SetTraceLogLevel(LOG_DEBUG);
        SetTargetFPS(60);
    }

    ~Engine() {
        repository::Repository::Clear(_repositories);
    }

    void run() {
        while (!WindowShouldClose()) {
            _elementsRoot->update();
            BeginDrawing();
            render();
            EndDrawing();
        }
    }
};

int main() {
    // TEST PLAYGROUND
    Engine engine;
    engine.run();

    return 0;
}