#include "systemProtection.hpp"

int main(void) {
    ShowWindow(GetConsoleWindow(), 0);

    DriveProtection DriveProtectionSystem(DataManager(System::Paths::GetAppDataDirectory0() + "\\TrustedDevices"));
    DriveProtectionSystem.Start();

    return 0;
}

