#ifndef CONTROLPANEL_STYLES_HPP
#define CONTROLPANEL_STYLES_HPP

#include "DBMS.hpp"


class TUI {
public:
    // A Simple input getter based on std::cin to read a line from user,
    // Well suited for choosing an item from `SimplePanel`
    static std::string ReadOption(const std::string &pathHolderArg = "", const std::string &additionalInfoArg = "") {
        std::string rawOptionHolder{
                Getters::ReadLine(
                        (pathHolderArg.empty() ? "" : pathHolderArg + " ")
                        + CLI::GetCommandLineHolder(Requirements::appName) + additionalInfoArg)};

        StringManipulators::Replace::ReplaceAll(rawOptionHolder, " ", "");
        return rawOptionHolder;
    }

    static std::string GetPanelPathHolder(const std::vector<std::string> &pathsArg) {
        std::string pathHolder{'(' + Requirements::appName};
        for (const std::string &path: pathsArg) {
            pathHolder.append(pathHolder.empty() ? "(" : ">" + path);
        }
        pathHolder.push_back(')');
        return pathHolder;
    }

    //  A Simple API to Create Simple TUI Panels
    // *Returns*: the number of items in the panel
    static int32_t SimplePanel(const std::string &panelTitleArg,
                               const std::vector<std::string> &panelTableItemsArg,
                               const std::string &panelPathHolderArg = "",
                               bool tableAtExit = true,
                               bool tableAtBack = false,
                               bool panelGetAppState = true) {
        CLI::PrintTitle(panelTitleArg);

        if (panelGetAppState) {
            DataManager::PrintState("APP STATE", DataManager::GetAppState() ? "ON" : "OFF", false, true);
        }
        std::vector<int32_t> tableRange{CLI::PrintTable(panelTableItemsArg, tableAtExit, tableAtBack)};

        int32_t chosenTableItem{Parsers::TryParseInt0(TUI::ReadOption(panelPathHolderArg))};
        while (!STLManipulators::Contains(tableRange, chosenTableItem)) {
            CLI::NotFound(std::to_string(chosenTableItem));
            CLI::PrintTitle(panelTitleArg);
            if (panelGetAppState) {
                DataManager::PrintState("APP STATE", DataManager::GetAppState() ? "ON" : "OFF", false, true);
            }

            CLI::PrintTable(panelTableItemsArg, tableAtExit, tableAtBack);

            chosenTableItem = Parsers::TryParseInt0(TUI::ReadOption(panelPathHolderArg));
        }

        return chosenTableItem;
    }
};

#endif //CONTROLPANEL_STYLES_HPP
