#pragma once

#include <Collision/PolygonalCollider.hpp>
#include <Graphics/LevelSprite.hpp>
#include <Scene/Camera.hpp>
#include <Script/GameObject.hpp>

namespace obe
{
    namespace Scene
    {
        void loadWorldLib(kaguya::State* lua);
        void loadWorldScriptEngineBaseLib(kaguya::State* lua);

        /**
         * \brief The Scene class is a container of all the game elements
         * @Bind
         */
        class Scene
        {
        private:
            std::string m_levelName = "";
            std::string m_baseFolder = "";
            Camera m_camera;
            bool m_cameraLocked = true;
            Transform::UnitVector m_cameraInitialPosition;
            bool m_updateState = true;
            bool m_needToOrderUpdateArray = true;

            std::vector<std::unique_ptr<Graphics::LevelSprite>> m_spriteArray;
            std::vector<std::unique_ptr<Collision::PolygonalCollider>> m_colliderArray;
            std::map<std::string, std::unique_ptr<Script::GameObject>> m_gameObjectMap;
            std::vector<Script::GameObject*> m_updateObjArray;
            std::vector<std::string> m_scriptArray;

            std::map<std::string, bool> m_showCollisionModes;
            void orderGameObjectExecutionByPriority();
            void displaySprites(sf::RenderWindow& target);
        public:
            /**
             * \brief Creates a new Scene
             */
            Scene();
            /**
             * \brief Default destructor of Scene (Removes Map Namespace in TriggerDatabase)
             */
            ~Scene();

            /**
             * \brief Loads the Scene from a .map.vili file
             * \param filename Name of the file located in Data/Maps (using System::Loaders)
             */
            void loadFromFile(const std::string& filename);
            /**
             * \brief Removes all elements in the Scene
             */
            void clearWorld();
            /**
             * \brief Dumps all elements of the Scene in a vili tree
             * \return 
             */
            vili::ViliParser* dump();
            /**
             * \brief Updates all elements in the Scene
             * \param dt DeltaTime used to update the Scene
             */
            void update(double dt);
            /**
             * \brief Display all elements of the Scene of target
             * \param target sf::RenderWindow you want to render all Scene elements to
             */
            void display(sf::RenderWindow& target);
            /**
             * \brief Get the name of the level
             * \return A std::string containing the name of the level
             */
            std::string getLevelName() const;
            /**
             * \brief Sets the name of the level
             * \param newName A std::string containing the new name of the level
             */
            void setLevelName(const std::string& newName);
            /**
             * \brief Enables or disables the Scene update
             * \param state true if the Scene should update, false otherwise
             */
            void setUpdateState(bool state);

            //GameObjects
            /**
             * \brief Creates a new GameObject
             * \param id Id of the new GameObject
             * \param object Type of the GameObject
             * \return A pointer to the newly created GameObject
             */
            Script::GameObject* createGameObject(const std::string& id, const std::string& object);
            /**
             * \brief Get how many GameObjects are present in the Scene
             * \return An unsigned int containing how many GameObjects are present in the Scene
             */
            unsigned int getGameObjectAmount() const;
            /**
             * \brief Get all the GameObjects present in the Scene
             * \return 
             */
            std::vector<Script::GameObject*> getAllGameObjects();
            /**
             * \brief Get a GameObject by Id (Raises an exception if not found)
             * \param id Id of the GameObject to retrieve
             * \return A pointer to the GameObject
             */
            Script::GameObject* getGameObjectById(const std::string& id);
            /**
             * \brief Check if a GameObject exists in the Scene
             * \param id Id of the GameObject to check the existence
             * \return true if the GameObject exists in the Scene, false otherwise
             */
            bool doesGameObjectExists(const std::string& id);
            /**
             * \brief Removes a GameObject from the Scene
             * \param id Id of the GameObject to remove from the Scene
             */
            void removeGameObjectById(const std::string& id);

            //Camera
            Camera* getCamera(); // <REVISION>
            void setCameraLock(bool state);
            bool isCameraLocked() const;

            //LevelSprites
            /**
             * \brief Reorganize all the LevelSprite (by Layer and z-depth)
             */
            void reorganizeLayers();
            /**
             * \brief Creates a new LevelSprite
             * \param id Id of the new LevelSprite
             * \return A pointer to the newly created LevelSprite
             */
            Graphics::LevelSprite* createLevelSprite(const std::string& id);
            /**
            * \brief Get how many LevelSprites are present in the Scene
            * \return An unsigned int containing how many LevelSprites are present in the Scene
            */
            unsigned int getLevelSpriteAmount() const;
            /**
             * \brief Get all the LevelSprites present in the Scene
             * \return A std::vector of LevelSprites pointer
             */
            std::vector<Graphics::LevelSprite*> getAllLevelSprites();
            /**
             * \brief Get all the LevelSprites present in the Scene in the given layer
             * \param layer Layer to get all the LevelSprites from
             * \return A std::vector of LevelSprites pointer
             */
            std::vector<Graphics::LevelSprite*> getLevelSpritesByLayer(int layer);
            /**
             * \brief Get the first found LevelSprite with the BoundingRect including the given position
             * \param position Position to check
             * \param layer Layer where to check
             * \return The pointer to a LevelSprite if found, nullptr otherwise
             */
            Graphics::LevelSprite* getLevelSpriteByPosition(const Transform::UnitVector& position, int layer);
            /**
             * \brief Get a LevelSprite by Id (Raises an exception if not found)
             * \param id Id of the LevelSprite to get
             * \return A pointer to the LevelSprite
             */
            Graphics::LevelSprite* getLevelSpriteById(const std::string& id);
            /**
             * \brief Check if a LevelSprite exists in the Scene
             * \param id Id of the LevelSprite to check the existence
             * \return true if the LevelSprite exists in the Scene, false otherwise
             */
            bool doesLevelSpriteExists(const std::string& id);
            /**
             * \brief Removes the LevelSprite with the given Id
             * \param id Id of the LevelSprite to remove
             */
            void removeLevelSpriteById(const std::string& id);

            //Colliders
            /**
             * \brief Creates a new Collider
             * \param id Id of the new Collider
             * \return A pointer to the newly created Collider
             */
            Collision::PolygonalCollider* createCollider(const std::string& id);
            /**
             * \brief Get how many Colliders are present in the Scene
             * \return The amount of Colliders present in the Scene
             */
            unsigned int getColliderAmount() const;
            /**
             * \brief Get all the pointers of the Colliders in the Scene
             * \return A std::vector containing all the pointers of the Colliders present in the Scene
             */
            std::vector<Collision::PolygonalCollider*> getAllColliders() const;
            /**
             * \brief Get the first Collider found with a point on the given position
             * \param position Position to get the Point of a Collider 
             * \return A std::pair containing the pointer to the Collider with a point at the given position and the index of the point
             */
            std::pair<Collision::PolygonalCollider*, int> getColliderPointByPosition(const Transform::UnitVector& position);
            /**
             * \brief Get the Collider using the centroid Position
             * \param position Position to check
             * \return A Pointer to the Collider if found, nullptr otherwise
             */
            Collision::PolygonalCollider* getColliderByCentroidPosition(const Transform::UnitVector& position);
            /**
             * \brief Get the Collider with the given Id (Raises an exception if not found)
             * \param id Id of the Collider to retrieve
             * \return A pointer to the Collider
             */
            Collision::PolygonalCollider* getColliderById(const std::string& id);
            /**
             * \brief Check the existence of the Collider with given Id in the Scene
             * \param id Id of the Collider to check the existence
             * \return true if the Collider was found, false otherwise
             */
            bool doesColliderExists(const std::string& id);
            /**
             * \brief Removes the Collider with the given Id from the Scene
             * \param id Id of the Collider to remove
             */
            void removeColliderById(const std::string& id);

            //Other
            /**
             * \brief Folder where was the map loaded with loadFromFile method
             * \return A std::string containing the folder where was loaded the map file
             */
            std::string getBaseFolder() const;
            /**
             * \brief Draws some elements of the Colliders
             * \param drawLines Draw the lines of the Colliders
             * \param drawPoints Draw the points of the Colliders
             * \param drawMasterPoint Draw the Centroid of the Colliders
             * \param drawSkel Draw the Skeleton of the Colliders
             */
            void enableShowCollision(bool drawLines = false, bool drawPoints = false, bool drawMasterPoint = false, bool drawSkel = false);
        };
    }
}
