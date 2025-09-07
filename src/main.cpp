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
    /**
     * RAII window initialization/destrucation
     * makes sure to run only after every other attributes have been destroyed
     */
    struct WindowInitialization {
        WindowInitialization() {
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
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
    bool _requestNewFrame;
    std::shared_ptr<ui::element::Element> _imgContainer; // TO REMOVE

    // created for debug purposes
    void _renderElements() {
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

    void scaffold() {
        /*
        Root {
            View {
                Rect {}
                View {
                    Image {}
                }
            }
        }
        */
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
        _imgContainer = imgContainer;
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
            // auto &transform = style.transform.emplace();
            // transform.rotation = {utils::AngleRadian(PI/3)};
            // auto &translation = transform.translation.emplace();
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

    void render() {
        BeginDrawing();
        _stackingContextRoot->renderTree();
        _layerRoot->composite();
        _layerRoot->render();
        EndDrawing();
    }

  public:
    Engine() {
        _repositories = repository::InitRepositories();
        scaffold();

        _stackingContextRoot = ui::rendering::StackingContext::BuildTree(_elementsRoot);
        _layerRoot = ui::rendering::Layer::BuildTree(_stackingContextRoot);
        _requestNewFrame = true;

        SetTraceLogLevel(LOG_DEBUG);
        SetTargetFPS(60);
    }

    ~Engine() {
        repository::Repository::Clear(_repositories);
    }

    void run() {
        while (!WindowShouldClose()) {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                auto style = _imgContainer->getStyle();
                style.opacity = 0.125;
                _imgContainer->updateStyle(style);
            } else if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
                auto style = _imgContainer->getStyle();
                style.opacity = 1;
                _imgContainer->updateStyle(style);
            }
            
            if (IsWindowResized()) {
                _elementsRoot->onWindowResized(GetScreenWidth(), GetScreenHeight());
            }

            // update inherited properties
            // and check for layout dirty flag
            _elementsRoot->update();

            if (_requestNewFrame) {
                render();
                // _requestNewFrame = false;
            } else {
                // prevent window from freezing
                PollInputEvents();
            }
        }
    }
};

int main() {
    // TEST PLAYGROUND
    Engine engine;
    engine.run();

    return 0;
}