#include <TGUI/TGUI.hpp>

#include <Collision/PolygonalCollider.hpp>
#include <Debug/Console.hpp>
#include <Editor/CollidersEditActions.hpp>
#include <Editor/EditorGUI.hpp>
#include <Editor/Grid.hpp>
#include <Editor/MapEditor.hpp>
#include <Editor/MapEditorTools.hpp>
#include <Editor/EditorGlobalActions.hpp>
#include <Editor/SpriteEditActions.hpp>
#include <Input/InputManager.hpp>
#include <Graphics/DrawUtils.hpp>
#include <Network/Network.hpp>
#include <Scene/Scene.hpp>
#include <Script/GameObject.hpp>
#include <Script/GlobalState.hpp>
#include <Script/Script.hpp>
#include <System/Cursor.hpp>
#include <System/Loaders.hpp>
#include <Time/FramerateCounter.hpp>
#include <Time/FramerateManager.hpp>
#include <Transform/UnitVector.hpp>
#include <Triggers/TriggerDatabase.hpp>
#include <Utils/MathUtils.hpp>

namespace obe
{
    namespace Editor
    {
        void editMap(const std::string& mapName)
        {
            std::cout << "<System> Creating window with resolution " << Transform::UnitVector::Screen.w << "x" << Transform::UnitVector::Screen.h << std::endl;
            
            //Creating Window
            sf::RenderWindow window(sf::VideoMode(Transform::UnitVector::Screen.w, Transform::UnitVector::Screen.h), "ObEngine", sf::Style::Fullscreen);
            window.setKeyRepeatEnabled(false);
            sf::Texture loadingTexture;
            loadingTexture.loadFromFile("Sprites/Menus/loading.png");
            loadingTexture.setSmooth(true);
            sf::Sprite loadingSprite;
            loadingSprite.setTexture(loadingTexture);
            sf::Font loadingFont;
            loadingFont.loadFromFile("Data/Fonts/weblysleekuil.ttf");
            sf::Text loadingText;
            loadingText.setFont(loadingFont);
            loadingText.setCharacterSize(70.0);
            loadingText.setPosition(348.0, 595.0);
            vili::ViliParser loadingStrDP("Sprites/Menus/loading.vili");
            std::string loadingRandomStr = loadingStrDP.at<vili::ArrayNode>("Loading", "loadingStr").get(
                Utils::Math::randint(0, loadingStrDP.at<vili::ArrayNode>("Loading", "loadingStr").size() - 1));
            loadingText.setString(loadingRandomStr);
            window.draw(loadingSprite);
            window.draw(loadingText);
            window.display();

            Script::hookCore.dropValue("TriggerDatabase", Triggers::TriggerDatabase::GetInstance());

            //Editor Triggers
            Triggers::TriggerGroupPtr editorTriggers = Triggers::TriggerDatabase::GetInstance()->createTriggerGroup("Global", "Editor");
            //Editor Collider Triggers
            editorTriggers
                ->addTrigger("ColliderCreated")
                ->addTrigger("ColliderRemoved")
                ->addTrigger("ColliderPicked")
                ->addTrigger("ColliderMoved")
                ->addTrigger("ColliderReleased")
                ->addTrigger("ColliderPointCreated")
                ->addTrigger("ColliderPointRemoved")
                ->addTrigger("ColliderPointPicked")
                ->addTrigger("ColliderPointMoved")
                ->addTrigger("ColliderPointReleased");
            //Editor Various Triggers
            editorTriggers
                ->addTrigger("CameraMoved")
                ->addTrigger("GridCursorMoved")
                ->addTrigger("CursorMagnetized")
                ->addTrigger("GridToggled")
                ->addTrigger("GridSnapToggled")
                ->addTrigger("CameraModeChanged")
                ->addTrigger("EditModeChanged")
                ->addTrigger("SceneSaved");
            //Editor Sprite Triggers
            editorTriggers
                ->addTrigger("SpriteZDepthChanged")
                ->addTrigger("SpriteLayerChanged")
                ->addTrigger("SpriteHandlePointPicked")
                ->addTrigger("SpriteHandlePointMoved")
                ->addTrigger("SpriteHandlePointReleased")
                ->addTrigger("SpriteCreated")
                ->addTrigger("SpriteSelect")
                ->addTrigger("SpriteMoved")
                ->addTrigger("SpriteUnselect")
                ->addTrigger("SpriteRemoved");

            //Game Triggers
            Triggers::TriggerGroupPtr gameTriggers = Triggers::TriggerDatabase::GetInstance()->createTriggerGroup("Global", "Game")
                ->addTrigger("Start")
                ->trigger("Start")
                ->addTrigger("End")
                ->addTrigger("Update")
                ->addTrigger("Render");

            //Console
            Debug::Console gameConsole;
            bool oldConsoleVisibility = false;
            std::vector<std::string> backupContexts;
            Script::hookCore.dropValue("Console", &gameConsole);

            //Font
            sf::Font font;
            font.loadFromFile("Data/Fonts/arial.ttf");

            //Config
            vili::ViliParser configFile;
            System::Path("Data/config.cfg.vili").loadResource(&configFile, System::Loaders::dataLoader);
            vili::ComplexNode& gameConfig = configFile->at("GameConfig");
            int scrollSensitive = gameConfig.at<vili::DataNode>("scrollSensibility");

            //Cursor
            System::Cursor cursor;
            Script::hookCore.dropValue("Cursor", &cursor);

            //Scene Creation / Loading
            Scene::Scene scene;
            //scene.getCamera()->bindView(window);
            Script::ScriptEngine["stream"] = gameConsole.createStream("Scene", true);
            Script::ScriptEngine.setErrorHandler([&gameConsole](int statuscode, const char* message)
            {
                gameConsole.pushMessage("LuaError", std::string("<Main> :: ") + message, sf::Color::Red);
                std::cout << "[LuaError]<Main> : " << "[CODE::" << statuscode << "] : " << message << std::endl;
            });
            Script::hookCore.dropValue("Scene", &scene);

            //Socket
            Network::NetworkHandler networkHandler;

            //Keybinding
            Input::InputManager inputManager;
            Script::hookCore.dropValue("InputManager", &inputManager);
            inputManager.configure(configFile.at("KeyBinding"));
            inputManager
                .addContext("game")
                .addContext("mapEditor")
                .addContext("gameConsole");

            //Editor Grid
            EditorGrid editorGrid(32, 32);
            Script::hookCore.dropValue("Grid", &editorGrid);
            inputManager.getAction("MagnetizeUp").setRepeat(200);
            inputManager.getAction("MagnetizeDown").setRepeat(200);
            inputManager.getAction("MagnetizeLeft").setRepeat(200);
            inputManager.getAction("MagnetizeRight").setRepeat(200);

            //GUI
            sf::Event event;
            tgui::Gui gui(window);
            gui.setFont("Data/Fonts/weblysleekuil.ttf");
            tgui::Panel::Ptr mainPanel = tgui::Panel::create();
            GUI::init(window);
            int saveEditMode = -1;
            gui.add(mainPanel);
            mainPanel->setSize(window.getSize().x, window.getSize().y);

            GUI::buildEditorMenu(mainPanel);
            GUI::buildObjectCreationMenu(mainPanel);

            tgui::Panel::Ptr editorPanel = gui.get<tgui::Panel>("editorPanel");
            tgui::Panel::Ptr mapPanel = gui.get<tgui::Panel>("mapPanel");
            tgui::Panel::Ptr settingsPanel = gui.get<tgui::Panel>("settingsPanel");
            tgui::Panel::Ptr spritesPanel = gui.get<tgui::Panel>("spritesPanel");
            tgui::Panel::Ptr objectsPanel = gui.get<tgui::Panel>("objectsPanel");
            tgui::Panel::Ptr requiresPanel = gui.get<tgui::Panel>("requiresPanel");
            tgui::Scrollbar::Ptr spritesScrollbar = gui.get<tgui::Scrollbar>("spritesScrollbar");
            tgui::Scrollbar::Ptr objectsScrollbar = gui.get<tgui::Scrollbar>("objectsScrollbar");

            GUI::buildToolbar(mainPanel, editorPanel);

            tgui::ComboBox::Ptr editMode = gui.get<tgui::ComboBox>("editMode");
            tgui::ComboBox::Ptr cameraMode = gui.get<tgui::ComboBox>("cameraMode");

            GUI::buildEditorMapMenu(mapPanel, scene);
            GUI::buildEditorSettingsMenu(settingsPanel, editorGrid, cursor, editMode);
            GUI::buildEditorSpritesMenu(spritesPanel, spritesScrollbar);
            GUI::buildEditorObjectsMenu(objectsPanel, requiresPanel, objectsScrollbar);

            tgui::CheckBox::Ptr enableGridCheckbox = gui.get<tgui::CheckBox>("enableGridCheckbox");
            tgui::CheckBox::Ptr snapGridCheckbox = gui.get<tgui::CheckBox>("snapGridCheckbox");
            tgui::EditBox::Ptr mapNameInput = gui.get<tgui::EditBox>("mapNameInput");
            tgui::Label::Ptr savedLabel = gui.get<tgui::Label>("savedLabel");
            tgui::Label::Ptr infoLabel = gui.get<tgui::Label>("infoLabel");
            tgui::CheckBox::Ptr displayFramerateCheckbox = gui.get<tgui::CheckBox>("displayFramerateCheckbox");

            //Map Editor
            Graphics::LevelSprite* hoveredSprite = nullptr;
            Graphics::LevelSprite* selectedSprite = nullptr;
            Graphics::LevelSpriteHandlePoint* selectedHandlePoint = nullptr;
            int selectedSpriteOffsetX = 0;
            int selectedSpriteOffsetY = 0;
            int cameraSpeed = Transform::UnitVector::Screen.h;
            int currentLayer = 1;
            Collision::PolygonalCollider* selectedMasterCollider = nullptr;
            int colliderPtGrabbed = -1;
            bool masterColliderGrabbed = false;
            sf::Text sprInfo;
            sprInfo.setFont(font);
            sprInfo.setCharacterSize(16);
            sprInfo.setFillColor(sf::Color::White);
            sf::RectangleShape sprInfoBackground(sf::Vector2f(100, 160));
            sprInfoBackground.setFillColor(sf::Color(0, 0, 0, 200));
            double waitForMapSaving = -1;

            //Framerate / DeltaTime
            Time::FPSCounter fps;
            fps.loadFont(font);
            Time::FramerateManager framerateManager(window, gameConfig);
            window.setVerticalSyncEnabled(framerateManager.isVSyncEnabled());

            scene.loadFromFile(mapName);

            mapNameInput->setText(scene.getLevelName());

            //Connect InputManager Actions
            connectSaveActions(editorTriggers.get(), inputManager, mapName, scene, waitForMapSaving, savedLabel);
            connectCamMovementActions(editorTriggers.get(), inputManager, scene, cameraSpeed, framerateManager);
            connectGridActions(editorTriggers.get(), inputManager, enableGridCheckbox, snapGridCheckbox, cursor, editorGrid);
            connectMenuActions(inputManager, editMode, cameraMode, editorPanel);
            connectSpriteLayerActions(editorTriggers.get(), inputManager, selectedSprite, scene, currentLayer);
            connectSpriteActions(editorTriggers.get(), inputManager, hoveredSprite, selectedSprite, selectedHandlePoint,
                scene, cursor, editorGrid, selectedSpriteOffsetX, selectedSpriteOffsetY, sprInfo, sprInfoBackground);
            connectCollidersActions(editorTriggers.get(), inputManager, scene, cursor, colliderPtGrabbed, selectedMasterCollider, masterColliderGrabbed);
            connectGameConsoleActions(inputManager, gameConsole);
            inputManager.getAction("ExitEditor").connect([&window](const Input::InputActionEvent& event)
            {
                window.close();
            });

            auto editModeCallback = [&editorTriggers, &inputManager, editMode]()
            {
                editorTriggers->pushParameter("EditModeChanged", "mode", editMode->getSelectedItem());
                editorTriggers->trigger("EditModeChanged");
                if (editMode->getSelectedItem() == "LevelSprites")
                {
                    inputManager.addContext("spriteEditing");
                }
                else
                {
                    inputManager.removeContext("spriteEditing");
                }
                if (editMode->getSelectedItem() == "Collisions")
                {
                    inputManager.addContext("colliderEditing");
                }
                else
                {
                    inputManager.removeContext("colliderEditing");
                }
            };

            editMode->connect("itemselected", editModeCallback);

            std::cout << "=> LISTING GLOBAL NAMESPACE TRIGGERGROUP CONTENT" << std::endl;
            for (auto& truc : Triggers::TriggerDatabase::GetInstance()->getAllTriggersGroupNames("Global"))
            {
                std::cout << "GLOBAL POSITIONING NAMESPACE LISTING : " << truc << std::endl;
            }

            //Game Starts
            while (window.isOpen())
            {
                framerateManager.update();

                gameTriggers->pushParameter("Update", "dt", framerateManager.getGameSpeed());
                gameTriggers->trigger("Update");

                if (framerateManager.doRender())
                    gameTriggers->trigger("Render");

                if (waitForMapSaving >= 0)
                {
                    waitForMapSaving += framerateManager.getDeltaTime();
                    if (waitForMapSaving > 1 && waitForMapSaving < 2)
                    {
                        savedLabel->hideWithEffect(tgui::ShowAnimationType::SlideFromTop, sf::Time(sf::seconds(0.5)));
                        waitForMapSaving = 2;
                    }
                    else if (waitForMapSaving > 3)
                        waitForMapSaving = -1;
                }

                bool drawFPS = displayFramerateCheckbox->isChecked();

                if (editorPanel->isVisible() && saveEditMode < 0)
                {
                    saveEditMode = editMode->getSelectedItemIndex();
                    editMode->setSelectedItemByIndex(3);
                }
                else if (!editorPanel->isVisible() && saveEditMode > 0)
                {
                    editMode->setSelectedItemByIndex(saveEditMode);
                    saveEditMode = -1;
                }

                Transform::UnitVector pixelCamera = scene.getCamera()->getPosition().to<Transform::Units::WorldPixels>();
                //Updates
                if (!gameConsole.isVisible())
                {
                    if (cameraMode->getSelectedItem() == "Movable Camera")
                    {
                        scene.setCameraLock(true);
                    }
                    else
                    {
                        scene.setCameraLock(false);
                    }
                }

                if (oldConsoleVisibility != gameConsole.isVisible())
                {
                    std::cout << "Swap console Visibility" << std::endl;
                    if (oldConsoleVisibility)
                    {
                        inputManager.clearContexts();
                        for (std::string& context : backupContexts)
                        {
                            inputManager.addContext(context);
                        }
                        backupContexts.clear();
                    }
                    else
                    {
                        backupContexts = inputManager.getContexts();
                        inputManager.setContext("gameConsole");
                    }
                    oldConsoleVisibility = gameConsole.isVisible();
                }

                Transform::UnitVector cursCoord(cursor.getX() + pixelCamera.x, cursor.getY() + pixelCamera.y, Transform::Units::WorldPixels);
                //Sprite Editing
                if (editMode->getSelectedItem() == "LevelSprites")
                {
                    scene.enableShowCollision(true, true, false, false);

                    if (hoveredSprite == nullptr)
                    {
                        hoveredSprite = scene.getLevelSpriteByPosition(cursCoord, currentLayer);
                        if (hoveredSprite != nullptr && hoveredSprite != selectedSprite)
                        {
                            hoveredSprite = scene.getLevelSpriteByPosition(cursCoord, currentLayer);
                            hoveredSprite->setColor(sf::Color(0, 255, 255));
                            std::string sprInfoStr;
                            sprInfoStr = "Hovered Sprite : \n";
                            sprInfoStr += "    Id : " + hoveredSprite->getId() + "\n";
                            sprInfoStr += "    Name : " + hoveredSprite->getPath() + "\n";
                            sprInfoStr += "    Pos : " + std::to_string(hoveredSprite->getX()) + "," + std::to_string(hoveredSprite->getY()) + "\n";
                            sprInfoStr += "    Size : " + std::to_string(hoveredSprite->getWidth()) + "," + std::to_string(hoveredSprite->getHeight()) + "\n";
                            sprInfoStr += "    Rot : " + std::to_string(hoveredSprite->getRotation()) + "\n";
                            sprInfoStr += "    Layer / Z : " + std::to_string(hoveredSprite->getLayer()) + "," + std::to_string(hoveredSprite->getZDepth()) + "\n";
                            sprInfo.setString(sprInfoStr);
                            sprInfoBackground.setSize(sf::Vector2f(sprInfo.getGlobalBounds().width + 20, sprInfo.getGlobalBounds().height - 10));
                        }
                        else
                            hoveredSprite == nullptr;
                    }
                    else if (hoveredSprite != nullptr)
                    {
                        sprInfoBackground.setPosition(cursor.getX() + 40, cursor.getY());
                        sprInfo.setPosition(cursor.getX() + 50, cursor.getY());
                        bool outHover = false;
                        Graphics::LevelSprite* testHoverSprite = scene.getLevelSpriteByPosition(cursCoord, currentLayer);
                        if (testHoverSprite != hoveredSprite)
                            outHover = true;
                        if (outHover)
                        {
                            if (hoveredSprite != selectedSprite)
                                hoveredSprite->setColor(sf::Color::White);
                            hoveredSprite = nullptr;
                            sprInfo.setString("");
                        }
                    }                      
                }
                else
                {
                    if (selectedSprite != nullptr)
                        selectedSprite->setColor(sf::Color::White);
                    selectedSprite = nullptr;
                    hoveredSprite = nullptr;
                    selectedSpriteOffsetX = 0;
                    selectedSpriteOffsetY = 0;
                    sprInfo.setString("");
                }

                //Collision Edition
                if (editMode->getSelectedItem() == "Collisions")
                {
                    Transform::UnitVector cursCoord(cursor.getX() + pixelCamera.x, cursor.getY() + pixelCamera.y, Transform::Units::WorldPixels);

                    scene.enableShowCollision(true, true, true, true);
                    if (selectedMasterCollider != nullptr)
                    {
                        selectedMasterCollider->clearHighlights();
                        selectedMasterCollider->highlightLine(selectedMasterCollider->findClosestLine(cursCoord));
                    }     
                }

                //GUI Update
                infoLabel->setText(
                    "Cursor : (" +
                    std::to_string(cursor.getX()) + ", " + std::to_string(cursor.getY()) +
                    ")" +
                    std::string("   Camera : (") +
                    std::to_string(int(pixelCamera.x)) + ", " + std::to_string(int(pixelCamera.y)) +
                    ")" +
                    std::string("   Sum : (") +
                    std::to_string(int(pixelCamera.x) + int(cursor.getX())) + ", " + std::to_string(int(pixelCamera.y) + int(cursor.getY())) +
                    ")" +
                    std::string("   Layer : ") + std::to_string(currentLayer)
                );

                //Events
                scene.update(framerateManager.getGameSpeed());
                Triggers::TriggerDatabase::GetInstance()->update();
                inputManager.update();
                cursor.update();
                if (drawFPS) fps.uTick();
                if (drawFPS && framerateManager.doRender()) fps.tick();

                //Triggers Handling
                networkHandler.handleTriggers();
                //cursor.handleTriggers();
                inputManager.handleTriggers();

                while (window.pollEvent(event))
                {
                    switch (event.type)
                    {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::TextEntered:
                        if (gameConsole.isVisible())
                            gameConsole.inputKey(event.text.unicode);
                        break;
                    case sf::Event::MouseWheelMoved:
                        if (event.mouseWheel.delta >= scrollSensitive)
                        {
                            scene.getCamera()->scale(0.9, Transform::Referencial::Center);
                            gameConsole.scroll(-1);
                        }
                        else if (event.mouseWheel.delta <= -scrollSensitive)
                        {
                            scene.getCamera()->scale(1.1, Transform::Referencial::Center);
                            gameConsole.scroll(1);
                        }
                        break;
                    }
                    gui.handleEvent(event);
                }
                //Draw Everything Here
                if (framerateManager.doRender())
                {
                    window.clear();
                    scene.display(window);
                    pixelCamera = scene.getCamera()->getPosition().to<Transform::Units::WorldPixels>(); // Do it once (Grid Draw Offset) <REVISION>
                    //Show Collision
                    if (editMode->getSelectedItem() == "Collisions")
                        scene.enableShowCollision(true);
                    else
                        scene.enableShowCollision(false);
                    if (editorGrid.getState())
                        editorGrid.draw(window, cursor, pixelCamera.x, pixelCamera.y);
                    //HUD & GUI
                    if (sprInfo.getString() != "")
                    {
                        window.draw(sprInfoBackground);
                        window.draw(sprInfo);
                    }
                    gui.draw();
                    if (drawFPS)
                        fps.draw(window);

                    //Console
                    if (gameConsole.isVisible())
                        gameConsole.display(window);

                    window.display();
                }
            }
            gameTriggers->trigger("End");
            Triggers::TriggerDatabase::GetInstance()->update();
            scene.update(framerateManager.getGameSpeed());

            window.close();
            gui.removeAllWidgets();
        }
    }
}
