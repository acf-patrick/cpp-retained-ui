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
                ui::rendering::StackingContext::AppendChild(parentCtx, ctx);

                if (e == elementsRoot)
                    rootCtx = e->getStackingContext();
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
                ui::rendering::Layer::AppendChild(parentLayer, layer);

                if (ctx == rootCtx)
                    rootLayer = ctx->getLayer();
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
        ui::element::Element::AppendChild(_elementsRoot, view);

        {
            auto layout = view->getLayout();
            auto &flex = layout.flex.emplace();
            flex.justifyContent = ui::style::JustifyContent::Center;
            flex.alignItems = ui::style::Alignment::Center;
            flex.flex = 1.0;
            view->updateLayout(layout);
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
        ui::element::Element::AppendChild(button, text);

        repository::FontRepository::Get()->load("roboto", "Roboto-Regular.ttf");

        auto imgContainer = std::make_shared<ui::element::View>();
        ui::element::Element::AppendChild(view, imgContainer);
        {
            auto layout = imgContainer->getLayout();
            layout.spacing.emplace().border = 3;
            auto &size = layout.size.emplace();
            size.width = utils::Value(480);
            size.height = utils::Value(300);
            imgContainer->updateLayout(layout);

            auto style = imgContainer->getStyle();
            style.borderColor = WHITE;
            imgContainer->updateStyle(style);
        }

        auto image = std::make_shared<ui::element::Image>("assets/images/cat.png", "cat");
        ui::element::Element::AppendChild(imgContainer, image);
        {
            auto style = image->getStyle();
            style.opacity = 0.125;
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