#include <Script/ViliLuaBridge.hpp>
#include <Utils/StringUtils.hpp>
#include <Utils/VectorUtils.hpp>

namespace obe
{
    namespace Script
    {
        namespace DataBridge
        {
            void dataToLua(kaguya::LuaTable& target, vili::Node* convert)
            {
                if (convert->getType() == vili::NodeType::ArrayNode)
                {
                    arrayNodeToLuaTable(target, static_cast<vili::ArrayNode*>(convert));
                }
                else if (convert->getType() == vili::NodeType::ComplexNode)
                {
                    complexNodeToLuaTable(target, static_cast<vili::ComplexNode*>(convert));
                }
                else if (convert->getType() == vili::NodeType::DataNode)
                {
                    dataNodeToLuaElement(target, static_cast<vili::DataNode*>(convert));
                }
            }

            vili::Node* luaToData(kaguya::LuaRef& convert)
            {
                //return Attribute();
                return nullptr;
            }

            void complexNodeToLuaTable(kaguya::LuaTable& target, vili::ComplexNode* convert)
            {
                target[convert->getId()] = kaguya::NewTable();
                kaguya::LuaTable injectTable = target[convert->getId()];
                kaguya::State olol;
                for (std::string elementName : convert->getAll())
                {
                    if (convert->getNodeType(elementName) == vili::NodeType::DataNode)
                    {
                        dataNodeToLuaElement(injectTable, &convert->getDataNode(elementName));
                    }
                    else if (convert->getNodeType(elementName) == vili::NodeType::ComplexNode)
                    {
                        complexNodeToLuaTable(injectTable, &convert->getComplexNode(elementName));
                    }
                    else if (convert->getNodeType(elementName) == vili::NodeType::ArrayNode)
                    {
                        arrayNodeToLuaTable(injectTable, &convert->getArrayNode(elementName));
                    }
                }
                target[convert->getId()] = injectTable;
            }

            void dataNodeToLuaElement(kaguya::LuaTable& target, vili::DataNode* convert)
            {
                if (convert->getDataType() == vili::DataType::Int)
                    target[convert->getId()] = convert->get<int>();
                else if (convert->getDataType() == vili::DataType::String)
                    target[convert->getId()] = convert->get<std::string>();
                else if (convert->getDataType() == vili::DataType::Bool)
                    target[convert->getId()] = convert->get<bool>();
                else if (convert->getDataType() == vili::DataType::Float)
                    target[convert->getId()] = convert->get<double>();
            }

            void arrayNodeToLuaTable(kaguya::LuaTable& target, vili::ArrayNode* convert)
            {
                kaguya::LuaTable injectList;
                for (int i = 0; i < convert->size(); i++)
                {
                    injectList[i] = &convert->get(i);
                }
                target[convert->getId()] = injectList;
            }

            vili::ComplexNode* luaTableToComplexNode(const std::string& id, kaguya::LuaRef& convert)
            {
                std::cout << "Convert table : " << id << std::endl;
                vili::ComplexNode* returnElement = nullptr;
                if (convert.type() == 0 || convert.type() == 5)
                {
                    returnElement = new vili::ComplexNode(id);
                    std::map<std::string, kaguya::LuaRef> tableMap = convert;
                    for (std::pair<std::string, kaguya::LuaRef> tableItem : tableMap)
                    {
                        std::string tableKey = tableItem.first;
                        if (convert[tableKey].type() == 5)
                        {
                            std::cout << "Push subtable : " << tableKey << std::endl;
                            kaguya::LuaRef tempTableRef = convert[tableKey];
                            returnElement->pushComplexNode(luaTableToComplexNode(tableKey, tempTableRef));
                        }
                        else if (Utils::Vector::isInList(convert[tableKey].type(), std::vector<int>({1, 3, 4})))
                        {
                            std::cout << "Push subelement : " << tableKey << std::endl;
                            kaguya::LuaRef tempElemRef = convert[tableKey];
                            returnElement->pushDataNode(luaElementToDataNode(tableKey, tempElemRef));
                        }
                    }
                }
                else
                    throw aube::ErrorHandler::Raise("ObEngine.Script.ViliLuaBridge.NotATable", { { "id", id } });
                return returnElement;
            }

            vili::DataNode* luaElementToDataNode(const std::string& id, kaguya::LuaRef& convert)
            {
                vili::DataNode* returnAttribute = nullptr;
                if (convert.type() == 3 && Utils::String::isStringInt(convert))
                {
                    returnAttribute = new vili::DataNode(id, vili::DataType::Int);
                    returnAttribute->set(int(convert));
                }
                else if (convert.type() == 3 && Utils::String::isStringFloat(convert))
                {
                    returnAttribute = new vili::DataNode(id, vili::DataType::Float);
                    returnAttribute->set(double(convert));
                }
                else if (convert.type() == 1)
                {
                    returnAttribute = new vili::DataNode(id, vili::DataType::Bool);
                    returnAttribute->set(bool(convert));
                }
                else if (convert.type() == 4)
                {
                    returnAttribute = new vili::DataNode(id, vili::DataType::String);
                    const char* convertChar = convert;
                    returnAttribute->set(convertChar);
                }

                return returnAttribute;
            }

            vili::ArrayNode* luaTableToArrayNode(const std::string& id, kaguya::LuaTable& convert)
            {
                /*ListAttribute* returnElement = new ListAttribute(id, dataType);
                std::cout << "Table Type : " << convert.type() << std::endl;
                if (convert.type() == 0 || convert.type() == 5) {
                    std::map<std::string, kaguya::LuaRef> tableMap = convert;
                    for (std::pair<std::string, kaguya::LuaRef> tableItem : tableMap) {
                        std::string tableKey = tableItem.first;
                        std::cout << "Current Table Key : " << tableKey << std::endl;
                        if (convert[tableKey].type() == 5) {
                            kaguya::LuaRef tempTableRef = convert[tableKey];
                            returnElement->pushComplexAttribute(luaTableToComplexAttribute(tableKey, tempTableRef));
                        }
                        else if (Utils::Vector::isInList(convert[tableKey].type(), std::vector<int>({ 1, 3, 4 }))) {
                            kaguya::LuaRef tempElemRef = convert[tableKey];
                            returnElement->pushBaseAttribute(luaElementToBaseAttribute(tableKey, tempElemRef));
                        }
                    }
                }
                else {
                    std::cout << "<Error:ViliLuaBridge:DataBridge>[luaTableToComplexAttribute] LuaElement " << id << " is not a table" << std::endl;
                }
                return returnElement;*/
                return nullptr;
            }
        }
    }
}
