#ifndef CONTROLPANEL_RELATED_HPP
#define CONTROLPANEL_RELATED_HPP

#include "booster.hpp"
#include <map>

class Requirements {
public:
    static const inline std::string appName{"PlugProtector"};

    static const inline std::string defaultRegistryContainerName{"_internal"};
    static const inline std::string defaultRegistrySubContainer{"Control Panel\\Keyboard\\"};
    static const inline std::string defaultRegistryContainerPath{
            defaultRegistrySubContainer + defaultRegistryContainerName};

    static const inline std::string retrieveAllDevices_Command = "powershell.exe -command \"$hash = $null;$hash = @{};"
                                                                 "$usb_dev = Get-WmiObject win32_volume | where{$_.DriveType -eq '5' -or $_.DriveType -eq '3' -or $_.DriveType -eq '2'};"
                                                                 "$damn = 0;"
                                                                 "foreach ($p in $usb_dev)"
                                                                 "{"
                                                                 "If ($p.Capacity -eq $null -or $p.Capacity -lt 0) {$p.Capacity = 0};"
                                                                 "If ($p.FreeSpace -eq $null -or $p.FreeSpace -lt 0) {$p.FreeSpace = 0};"
                                                                 "If ($p.BlockSize -eq $null -or $p.BlockSize -lt 0) {$p.BlockSize = 0};"
                                                                 "$finallText = "
                                                                 "'{' + '$0L1F30${0}$0L1F30$: ' -f $p.DeviceID + "
                                                                 "'{'+'"
                                                                 "$0L1F30$SerialNumber$0L1F30$: $0L1F30${0}$0L1F30$, "
                                                                 "$0L1F30$Name$0L1F30$: $0L1F30${1}$0L1F30$, "
                                                                 "$0L1F30$Label$0L1F30$: $0L1F30${2}$0L1F30$, "
                                                                 "$0L1F30$DriveType$0L1F30$: $0L1F30${3}$0L1F30$, "
                                                                 "$0L1F30$DriveLetter$0L1F30$: $0L1F30${4}$0L1F30$, "
                                                                 "$0L1F30$Caption$0L1F30$: $0L1F30${5}$0L1F30$, "
                                                                 "$0L1F30$Capacity$0L1F30$: {6}, "
                                                                 "$0L1F30$FreeSpace$0L1F30$: {7}, "
                                                                 "$0L1F30$FileSystem$0L1F30$: $0L1F30${8}$0L1F30$, "
                                                                 "$0L1F30$BlockSize$0L1F30$: {9}'"
                                                                 " -f $p.SerialNumber, $p.Name, $p.Label, $p.DriveType, "
                                                                 "$p.DriveLetter, $p.Caption, $p.Capacity, $p.FreeSpace, "
                                                                 "$p.FileSystem, $p.BlockSize + "
                                                                 "'}' + "
                                                                 "'}' ; "
                                                                 "$hash.add($damn, $finallText); $damn+=1};"
                                                                 "$damn=0;"
                                                                 "$all_data = $hash | Select-Object -Property *;"
                                                                 "echo $all_data.Values;\"";
};

struct deviceStructure {
public:
    std::string ID;
    std::string SerialNumber;
    std::string Name;
    std::string Label;
    std::string DriveType;
    std::string FileSystem;
    std::string DriveLetter;
    std::string Caption;
    int64_t Capacity{0};
    int64_t FreeSpace{0};
    int32_t BlockSize{0};
    uint32_t BytesPerSector{512};

    deviceStructure(std::string idArg, std::string serialNumberArg, std::string nameArg,
                    std::string labelArg, std::string driveTypeArg, std::string fileSystemArg,
                    std::string driveLetterArg, std::string captionArg, const int64_t &capacityArg,
                    const int64_t &freeSpaceArg, const int32_t &blockSizeArg, const uint32_t &bytesPerSectorArg = 512)
            : ID(std::move(idArg)), SerialNumber(std::move(serialNumberArg)), Name(std::move(nameArg)),
              Label(std::move(labelArg)), DriveType(std::move(driveTypeArg)), FileSystem(std::move(fileSystemArg)),
              DriveLetter(std::move(driveLetterArg)), Caption(std::move(captionArg)), Capacity(capacityArg),
              FreeSpace(freeSpaceArg), BlockSize(blockSizeArg), BytesPerSector(bytesPerSectorArg) {}
};

typedef std::map<std::string, deviceStructure> Devices;


#endif //CONTROLPANEL_RELATED_HPP
