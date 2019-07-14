class CfgPatches {
    class dedmen_ArmaFullDump {
        name = "ArmaFullDump";
        units[] = {};
        weapons[] = {};
        requiredVersion = 1.92;
        requiredAddons[] = {};
        author = "dedmen";
    };
};

//class Extended_PreStart_EventHandlers {
//    class dedmen_ArmaFullDump {
//        init = "'ArmaFullDump' callExtension 'enable';";
//    };
//};

class CfgFunctions {
    class ArmaFullDump {
        class startup {
            class myFunction {
                file = "ArmaFullDump\start.sqf";
                preStart = 1; //1 to call the function upon game start, before title screen, but after all addons are loaded (config.cpp only)
            };
        };
    };
};