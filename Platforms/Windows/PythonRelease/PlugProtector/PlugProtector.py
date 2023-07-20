import datetime
import os
import sys
import winreg
from ast import literal_eval
from getpass import getuser
from shutil import copy
from time import sleep

import pytz
import winshell
from win32com.client import Dispatch


class USBProtection:
    DAMN = 'OFF'
    REG_PATH = r"Control Panel\Keyboard"

    def set_reg(self, name, value, path=REG_PATH):
        try:
            value = str(value)
            winreg.CreateKey(winreg.HKEY_CURRENT_USER, path)
            registry_key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, path, 0,
                                          winreg.KEY_WRITE)
            winreg.SetValueEx(registry_key, name, 0, winreg.REG_SZ, value)
            winreg.CloseKey(registry_key)
            return True
        except WindowsError:
            return False

    def get_reg(self, name, path=REG_PATH):
        try:
            registry_key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, path, 0,
                                          winreg.KEY_READ)
            value, regtype = winreg.QueryValueEx(registry_key, name)
            winreg.CloseKey(registry_key)
            return value
        except WindowsError:
            return None

    def replace_all(self, text, dic):
        for i, j in dic.items():
            text = text.replace(i, j)
        return text

    @property
    def finder_command(self):
        filter_names = {
            '\n': '',
            '$0L1F30$': "'",
            '\\': '\\\\'
        }
        all_devs = {}
        [all_devs.update(literal_eval(self.replace_all(data, filter_names))) for data in os.popen(
            '''powershell -command "$hash = $null;$hash = @{};$usb_dev =
            gwmi win32_volume | where{$_.DriveType -eq '5' -or $_.DriveType -eq
            '3' -or $_.DriveType -eq '2'};$damn = 0;foreach ($p in $usb_dev){$finallText
            = '{' + '$0L1F30${0}$0L1F30$:' -f $p.DeviceID + '{'+'$0L1F30$SerialNumber$0L1F30$
            : $0L1F30${0}$0L1F30$, $0L1F30$Name$0L1F30$: $0L1F30${1}$0L1F30$ ,
            $0L1F30$Label$0L1F30$ : $0L1F30${2}$0L1F30$ ,$0L1F30$DriveType$0L1F30$:
            $0L1F30${3}$0L1F30$, $0L1F30$DriveLetter$0L1F30$: $0L1F30${4}$0L1F30$,
            $0L1F30$Caption$0L1F30$: $0L1F30${5}$0L1F30$, $0L1F30$Capacity$0L1F30$:
            $0L1F30${6}$0L1F30$, $0L1F30$FreeSpace$0L1F30$: $0L1F30${7}$0L1F30$,
            $0L1F30$FileSystem$0L1F30$: $0L1F30${8}$0L1F30$, $0L1F30$BlockSize$0L1F30$:
            $0L1F30${9}$0L1F30$' -f $p.SerialNumber, $p.Name, $p.Label, $p.DriveType,
            $p.DriveLetter, $p.Caption, $p.Capacity, $p.FreeSpace, $p.FileSystem,
            $p.BlockSize + '}' + '}' ; $hash.add($damn, $finallText); $damn+=1};$damn=0;
            $all_data = $hash | Select-Object -Property *;echo $all_data.Values;"''').readlines()]

        return all_devs

    def merge_data(self, *args):
        if len(args) == 7:
            return args[0] | args[1] | args[2] | args[3] | args[4] | args[5] | args[6]
        if len(args) == 6:
            return args[0] | args[1] | args[2] | args[3] | args[4] | args[5]
        if len(args) == 5:
            return args[0] | args[1] | args[2] | args[3] | args[4]
        if len(args) == 4:
            return args[0] | args[1] | args[2] | args[3]
        if len(args) == 3:
            return args[0] + args[1] + args[2]
        if len(args) == 2:
            return args[0] | args[1]
        else:
            return ''

    @property
    def all_devices(self):
        return self.finder_command

    # ------------------- CONFIGS-START -------------------
    def resource_path(self, relative_path):
        base_path = getattr(sys, '_MEIPASS', os.path.dirname(
            os.path.abspath(__file__)))
        return os.path.join(base_path, str(relative_path))

    def createShortcut(self, path, target='', wDir='', icon=''):
        ext = path[-3:]
        if ext == 'url':
            shortcut = open(path, 'w')
            shortcut.write('[InternetShortcut]\n')
            shortcut.write('URL=%s' % target)
            shortcut.close()
        else:
            shell = Dispatch('WScript.Shell')
            shortcut = shell.CreateShortCut(path)
            shortcut.Targetpath = target
            shortcut.WorkingDirectory = wDir
            if icon == '':
                pass
            else:
                shortcut.IconLocation = icon
            shortcut.save()

    def write_on_file(self, FileName, Content, Mod='w'):
        try:
            with open(f'{FileName}', f'{Mod}') as writer:
                writer.write(str(Content))
                writer.close()
        except:
            pass

    def check_file_existence(self, FileName):
        try:
            return os.path.exists(FileName)
        except:
            pass

    def multiplication(self, src='', dest=''):
        try:
            if src and dest:
                copy(src, dest)
            else:
                current_file_path = str(sys.argv[0])
                destination_file_path = r'C:\Users\%s\AppData\Roaming' % getuser()
                copy(current_file_path, destination_file_path)
        except:
            pass

    def pwd_fixer(self):
        path = r'C:\Users\%s\AppData\Roaming' % getuser()
        pwd = os.getcwd()
        if not self.check_file_existence(fr'{path}\usbProtection.exe'):
            print(f'MOVING DATA to \"{path}\" AND RE_EXECUTING...')
            self.multiplication()
            self.DAMN = 'ON'
            os.popen(fr'c:&"{path}\usbProtection.exe"')

    def shortcut_creator(self):
        startup = winshell.startup()
        self.createShortcut(fr'{startup}\usbProtection.lnk',
                            fr'C:\Users\{getuser()}\AppData\Roaming\usbProtection.exe')

    def cache_creator(self):
        if not self.check_file_existence(fr'C:\Users\{getuser()}\AppData\Roaming\off_on_state.txt'):
            self.write_on_file(
                fr'C:\Users\{getuser()}\AppData\Roaming\off_on_state.txt', 'YES')
        if not self.check_file_existence(fr'C:\Users\{getuser()}\AppData\Roaming\trustedDevices.txt'):
            self.write_on_file(
                fr'C:\Users\{getuser()}\AppData\Roaming\trustedDevices.txt', self.all_devices)
        if not self.check_file_existence(fr'C:\Users\{getuser()}\AppData\Roaming\untrustedDeviceLogs.txt'):
            self.write_on_file(
                fr'C:\Users\{getuser()}\AppData\Roaming\untrustedDeviceLogs.txt', '')
        if not self.check_file_existence(fr'{winshell.startup()}\usbProtection.lnk'):
            self.shortcut_creator()

    def convert_bytes(self, size):
        try:
            size = int(size)
            if size >= 1024 and size < 1024 * 1024:
                return f'{round(size / 1024, 1)}KB'
            elif size >= 1024 * 1024 and size < 1024 * 1024 * 1024:
                return f'{round(size / (1024 * 1024), 1)}MB'
            elif size >= 1024 * 1024 * 1024:
                return f'{round(size / (1024 * 1024 * 1024), 1)}GB'
            else:
                return f'{size}B'
        except:
            return '0'

    # ------------------- CONFIGS-END -------------------

    def not_trusted_devices(self, device_data):
        with open(fr'C:\Users\{getuser()}\AppData\Roaming\trustedDevices.txt', 'r') as trusted_devices:
            trusted_devices_dict = literal_eval(trusted_devices.read())
            not_trusted_devs = {}
            for dev_d in device_data:
                if dev_d in trusted_devices_dict:
                    pass
                else:
                    not_trusted_devs.update({dev_d: {'SerialNumber': device_data[dev_d]['SerialNumber'],
                                                     'Capacity': device_data[dev_d]['Capacity'],
                                                     'FreeSpace': device_data[dev_d]['FreeSpace'],
                                                     'Name': device_data[dev_d]['Name'],
                                                     'FileSystem': device_data[dev_d]['FileSystem'],
                                                     'BlockSize': device_data[dev_d]['BlockSize'],
                                                     'DriveType': device_data[dev_d]['DriveType']}})

            return not_trusted_devs

    @property
    def toronto_time(self):
        try:
            tor_time = str(datetime.datetime.now(
                pytz.timezone('America/Toronto')))
            tor_time = tor_time[:tor_time.rfind('.')]
        except:
            tor_time = str(datetime.datetime.now(
                pytz.timezone('America/Toronto')))
        return tor_time

    def not_trusted_device_fucker(self, not_trusted_devs):
        if len(not_trusted_devs) > 0:
            for n_dev, n_dev_data in not_trusted_devs.items():
                if str(n_dev_data["DriveType"]) == '2':
                    category = 'Removable storages(Like: USB, External Derives and etc.)'
                elif str(n_dev_data["DriveType"]) == '3':
                    category = 'Internal HardDisks(Like: HDD, SSD, M2 and etc.)'
                elif str(n_dev_data["DriveType"]) == '5':
                    category = 'Optical Disc Device(Like: CD, DVD-ROM and etc.)'
                else:
                    category = 'Unknown'

                untrusted_packet = f'[Name: {n_dev_data["Name"]}]
                [SerialNumber: {n_dev_data["SerialNumber"]}]
                [Capacity: {self.convert_bytes(n_dev_data["Capacity"])}]
                [FreeSpace: {self.convert_bytes(n_dev_data["FreeSpace"])}]
                [FormatType: {n_dev_data["FileSystem"]}][FormatBlockSize:{n_dev_data["BlockSize"]}]
                [Category: {category}] CONNECTED AT
                [{self.toronto_time} America/Toronto] AND KILLED SUCCESSFULLY!\n'

                self.write_on_file(fr'C:\Users\{getuser()}\AppData\Roaming\untrustedDeviceLogs.txt',
                                   untrusted_packet,
                                   'a+')
                os.popen(
                    r'powershell -command "$usbdev = gwmi win32_volume | where{$_.DeviceID -eq ' + f'\'{n_dev}\'' + '}; $usbdev.DriveLetter = $null;$usbdev.Put()"')
                sleep(2.5)
                os.popen(
                    fr'c:&C:\Users\{getuser()}\AppData\Roaming\RemoveDrive.exe {n_dev} -f')

    @property
    def scan_access(self):
        with open(fr'C:\Users\{getuser()}\AppData\Roaming\off_on_state.txt', 'r') as which_scan_access:
            access_mod = which_scan_access.read().replace(' ', '')
            if access_mod == 'NO':
                return False
            else:
                return True

    def executor(self):
        if self.get_reg('DoesUSBProtectorInstalled') != 'YES':
            self.set_reg('DoesUSBProtectorInstalled', 'YES')
        file_fix = 0
        while True:
            try:
                while not self.scan_access:
                    file_fix += 1
                    if file_fix >= 10:
                        self.cache_creator()
                        file_fix = 0
                    sleep(1)
                    print('ACCESS DENIED')
                while self.scan_access:
                    print('ACCESS SUCCESSFUL')
                    file_fix += 1
                    if file_fix >= 10:
                        self.cache_creator()
                        file_fix = 0
                    USBProtect.not_trusted_device_fucker(
                        USBProtect.not_trusted_devices(USBProtect.all_devices))
                    while not self.scan_access:
                        file_fix += 1
                        if file_fix >= 10:
                            self.cache_creator()
                            file_fix = 0
                        sleep(1)
                        print('ACCESS DENIED')
            except:
                continue


USBProtect = USBProtection()

USBProtect.shortcut_creator()
USBProtect.cache_creator()
USBProtect.pwd_fixer()

if USBProtect.DAMN == 'ON':
    pass
else:
    USBProtect.executor()
