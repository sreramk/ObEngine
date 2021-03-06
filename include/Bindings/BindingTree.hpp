#pragma once

#include <functional>
#include <vector>

#include <kaguya/kaguya.hpp>

#include <Types/Identifiable.hpp>

namespace obe
{
    namespace Bindings
    {
        extern std::function<void(kaguya::State*)> RegisterLib;

        class BindingTree : public Types::Identifiable
        {
        private:
            BindingTree* m_parent;
            std::vector<std::unique_ptr<BindingTree>> m_children;
            std::function<void(kaguya::State*)> m_lib;
            bool m_hasLib = false;
            std::string getNodePath() const;
        public:
            /**
             * \brief Builds a BindingTree Node with a lib
             * \param parent Parent of the BindingTree Node
             * \param id Id of the new BindingTree node
             * \param lib Lib of the BindingTree Node
             */
            BindingTree(BindingTree* parent, const std::string& id, std::function<void(kaguya::State*)> lib);
            /**
             * \brief Builds a BindingTree Node
             * \param parent Parent of the BindingTree Node
             * \param id Lib of the BindingTree Node
             */
            BindingTree(BindingTree* parent, const std::string& id);
            /**
             * \brief Builds a BindingTree root (without parent)
             * \param id Id of the BindingTree root
             */
            BindingTree(const std::string& id);
            /**
             * \brief Accesses a direct child of the BindingTree Node
             * \param id Id of the child to access
             * \return A reference to the child if found (raises an ObEngine.Bindings.BindingTree.ChildNotFound Exception otherwise)
             */
            BindingTree& operator[](const std::string& id);
            /**
             * \brief Adds a child with a lib to the BindingTree node
             * \param id Id of the BindingTree Node to add
             * \param lib Library of the new Node
             * \return A reference to the original BindingTree node (to chain calls)
             */
            BindingTree& add(const std::string& id, std::function<void(kaguya::State*)> lib);
            /**
             * \brief Adds a child to the BindingTree node
             * \param id Id of the BindingTree Node to add
             * \return A reference to the original BindingTree node (to chain calls)
             */
            BindingTree& add(const std::string& id);
            /**
             * \brief Walks to BindingTree node at path
             * \param path Path where to walk to (A std::vector of std::string containing ids of the Nodes forming the path)
             * \return A reference of the BindingTree node if found (raises an ObEngine.Bindings.BindingTree.ChildNotFound Exception otherwise)
             */
            BindingTree& walkTo(std::vector<std::string> path);
            /**
             * \brief Checks if the BindingTree node contains a child with the given id
             * \param id Id of the Node to check the existence
             * \return true if the Node exists in the BindingTree children, false otherwise
             */
            bool exists(const std::string& id);
            /**
             * \brief Loads the contained library
             * \param lua Lua VM where to load the library
             * \param spreads Should be equal to true if the Node will call operator() on all its children, false otherwise
             */
            void operator()(kaguya::State* lua, bool spreads = true);
        };
    }
}
