class CfgPatches {
    class dedmen_ArmaFullDump {
        name = "ArmaFullDump";
        units[] = {};
        weapons[] = {};
        requiredVersion = 1.92;
        requiredAddons[] = {"cba_main"};
        author = "dedmen";
    };
};

class Extended_PreStart_EventHandlers {
    class dedmen_ArmaFullDump {
        init = "'ArmaFullDump' callExtension 'enable';";
    };
};