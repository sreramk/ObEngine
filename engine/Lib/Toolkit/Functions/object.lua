local Color = require("Lib/StdLib/ConsoleColor");
local Route = require("Lib/Toolkit/Route");
local Style = require("Lib/Toolkit/Stylesheet")
local inspect = require("Lib/StdLib/Inspect");

local Functions = {};

function Functions.create(objectName, components)
    components = components:upper();
    local currentWs = Core.System.Path.Paths()[1].basePath;
    local haveAnimator = false;
    local haveLevelSprite = false;
    local haveCollider = false;
    local haveScript = false;
    Color.print({
        { text = "Creating GameObject <", color = Style.Execute},
        { text = objectName, color = Style.Object},
        { text = "> ...", color = Style.Execute}
    }, 1);
    os.execute(("mkdir " .. currentWs .. "/Data/GameObjects/" .. objectName):gsub("/", Core.Utils.File.separator()));
    local objSaveFile = Vili.ViliParser.new();
    objSaveFile:root():createComplexNode(objectName);
    if (components ~= nil) then
        if (string.find(components, "C") ~= nil) then
            haveCollider = true;
            Color.print({
                { text = "+ Using component ", color = Style.Success},
                { text = "(C)ollider", color = Style.Object}
            }, 2);
            objSaveFile:root():at(objectName):createComplexNode("Collider");
            objCollider = objSaveFile:root():at(objectName .. "/Collider");
            objCollider:createComplexNode("unit");
            objCollider:at("unit"):createDataNode("unit", "WorldUnits");
            objCollider:createArrayNode("points");
            objCollider:getArrayNode("points"):push(0);
            objCollider:getArrayNode("points"):push(0);
            objCollider:getArrayNode("points"):push(1);
            objCollider:getArrayNode("points"):push(0);
            objCollider:getArrayNode("points"):push(1);
            objCollider:getArrayNode("points"):push(1);
            objCollider:getArrayNode("points"):push(0);
            objCollider:getArrayNode("points"):push(1);
        end
        if (string.find(components, "A") ~= nil) then
            haveAnimator = true;
            objSaveFile:root():at(objectName):createComplexNode("Animator");
            objSaveFile:root():at(objectName .. "/" .. "Animator"):createDataNode("path",
            "Sprites/GameObjects/" .. objectName);
            Color.print({
                { text = "+ Using component ", color = Style.Success},
                { text = "(A)nimator", color = Style.Object}
            }, 2);
        end
        if (string.find(components, "L") ~= nil) then
            haveLevelSprite = true;
            os.execute(("mkdir " .. currentWs .. "/Sprites/GameObjects/" .. objectName):gsub("/", Core.Utils.File.separator()));
            Color.print({
                { text = "+ Using component ", color = Style.Success},
                { text = "(L)evelSprite", color = Style.Object}
            }, 2);
            objSaveFile:root():at(objectName):createComplexNode("LevelSprite");
            local objLevelSprite = objSaveFile:root():at(objectName .. "/" .. "LevelSprite");
            if (haveCollider) then
                objLevelSprite:createDataNode("position", "relative");
            else
                objLevelSprite:createDataNode("position", "absolute");
            end
            objLevelSprite:createDataNode("offsetX", 0);
            objLevelSprite:createDataNode("offsetY", 0);
            objLevelSprite:createDataNode("rotation", 0);
            objLevelSprite:createDataNode("scale", 1.0);
            objLevelSprite:createDataNode("layer", 1);
            objLevelSprite:createDataNode("z-depth", 0);
        end
        if (string.find(components, "S") ~= nil) then
            haveScript = true;
            local scriptFile = io.open((currentWs .. "/Data/GameObjects/" ..
                objectName .. "/" .. objectName .. ".lua"):gsub("/", Core.Utils.File.separator()), "w");
            scriptFile:write("function Local.Init()\n    print(\"Hello, World!\");\nend");
            scriptFile:close();
            Color.print({
                { text = "+ Using component ", color = Style.Success},
                { text = "(S)cript", color = Style.Object}
            }, 2);
            objSaveFile:root():at(objectName):createComplexNode("Script");
            objSaveFile:root():at(objectName .. "/Script"):createDataNode("priority", 0);
            objSaveFile:root():at(objectName .. "/Script"):createDataNode("source", "Data/GameObjects/" .. 
                objectName .. "/" .. objectName .. ".lua");
        end
    end
    objSaveFile:writeFile((currentWs .. "/Data/GameObjects/" ..
        objectName .. "/" .. objectName .. ".obj.vili"):gsub("/", Core.Utils.File.separator()));
    Color.print({
        { text = "GameObject <", color = Style.Success},
        { text = objectName, color = Style.Object},
        { text = "> has been successfully created !", color = Style.Success}
    }, 1);
end

function Functions.list()
    local allObjects = Core.System.Path("Data/GameObjects"):DirectoryListLoader();
    Color.print({
        { text = "Listing all objects : ", color = Style.Execute},
    }, 1);
    for _, key in pairs(allObjects) do 
        Color.print({
            { text = "- Object : ", color = Style.Default},
            { text = key, color = Style.Object}
        }, 2);
    end
end

return {
    Functions = Functions,
    Routes = {
        Route.Help("Commands to work with GameObjects");
        Route.Node("create", {
            Route.Help("Creates a new GameObject");
            Route.Arg("objectName", {
                Route.Arg("components", {
                    Route.Call("create");
                })
            })
        }),
        Route.Node("list", {
            Route.Help("List all existing GameObjects");
            Route.Call("list");
        });
    }
};