#include <iostream>
#include <memory>
#include <queue>
#include <raylib.h>
#include <repository.h>
#include <ui.h>
#include <unordered_set>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

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
        std::queue<std::shared_ptr<ui::element::Element>> queue;
        std::unordered_set<ui::element::Element::ElementId> visitedIds;

        queue.push(elementsRoot);
        while (!queue.empty()) {
            auto e = queue.front();
            queue.pop();
            if (visitedIds.contains(e->getId()))
                continue;

            if (ui::rendering::StackingContext::IsRequiredFor(e)) {
                auto ctx = std::make_shared<ui::rendering::StackingContext>(e);
                auto parentCtx = e->getParentStackingContext();
                e->setStackingContext(ctx);
                ui::rendering::StackingContext::AppendChild(parentCtx, ctx);
            } else {
                auto parentCtx = e->getParentStackingContext();
                parentCtx->addElement(e);
                e->setStackingContext(parentCtx);
            }

            for (auto child : e->getChildren())
                queue.push(child);

            visitedIds.emplace(e->getId());
        }

        return elementsRoot->getStackingContext();
    }

    std::shared_ptr<ui::rendering::Layer> buildLayerTree(std::shared_ptr<ui::rendering::StackingContext> rootCtx) const {
        std::queue<std::shared_ptr<ui::rendering::StackingContext>> queue;
        queue.push(rootCtx);

        while (!queue.empty()) {
            auto ctx = queue.front();
            queue.pop();
            auto ctxOwner = ctx->getOwner();

            if (ui::rendering::Layer::IsRequiredFor(ctxOwner)) {
                auto layer = std::make_shared<ui::rendering::Layer>(ctxOwner);
                auto parentLayer = ctx->getParentLayer();
                ctx->setLayer(layer);
                ui::rendering::Layer::AppendChild(parentLayer, layer);
            } else {
                ctx->setLayer(ctx->getParentLayer());
            }

            ctx->updateLayersElements();

            for (auto child : ctx->getChildren())
                queue.push(child);
        }

        return rootCtx->getLayer();
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
        std::unordered_set<ui::element::Element::ElementId> visitedIds;

        queue.push(_elementsRoot);
        while (!queue.empty()) {
            auto e = queue.front();
            queue.pop();
            if (visitedIds.contains(e->getId()))
                continue;

            e->render();
            visitedIds.emplace(e->getId());

            for (auto child : e->getChildren())
                queue.push(child);
        }
    }

  public:
    Engine() {
        _repositories = repository::InitRepositories();
        scaffold();

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
            renderElements();
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