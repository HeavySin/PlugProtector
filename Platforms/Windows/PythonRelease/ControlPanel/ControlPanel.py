from ast import literal_eval
from getpass import getuser
from msvcrt import getch
import string
import winreg
import time
import os


class File:
    def write_on_file(self, file_name, content, mod='w'):
        try:
            with open(f'{file_name}', f'{mod}') as writer:
                writer.write(str(content))
                writer.close()
        except:
            pass


class Regedit:
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


class Functions:
    @property
    def clear_terminal(self):
        os.system('cls' if os.name == 'nt' else 'clear')

    def press_enter_to_continue(self, show_text=False, text=None):
        if show_text and text:
            print(f'\n{text}')
        elif show_text:
            print('\nPress enter to continue')
        getch()

    def print_first_message_line_on_top(self, message=''):
        print(f'------------- {message.upper()} -------------\n\n')

    def has_only_latin_letters(self, name):
        char_set = string.printable
        return all((True if x in char_set else False for x in name))


class Panel:
    Funcs = Functions()
    Registers = Regedit()

    def wrong_time_password_sleep(self, wrong_times):
        wrong_times = int(wrong_times)

        def sleep_for_sec(secs):
            secs = int(secs)
            for _ in range(secs + 1):
                time.sleep(1)
                print(f'You can try again in {secs - _}.', end='\r')
            print('                                         ')

        if wrong_times < 6:
            sleep_for_sec(5)
        elif 6 <= wrong_times < 20:
            sleep_for_sec(15)
        elif 20 <= wrong_times < 50:
            sleep_for_sec(30)
        else:
            sleep_for_sec(60)

    def getFileContent(self, file_name):
        try:
            with open(f'{file_name}', 'r') as opened_file:
                file_content = opened_file.read()
                opened_file.close()
                return file_content
        except:
            return ''

    def write_on_file(self, FileName, Content, Mod='w'):
        try:
            with open(f'{FileName}', f'{Mod}') as writer:
                writer.write(str(Content))
                writer.close()
        except:
            pass

    def register(self):
        if self.Registers.get_reg('USBPanelKey') is not None:
            self.login()
        else:
            self.Funcs.clear_terminal
            self.Funcs.print_first_message_line_on_top('SIGNUP')
            print('Enter new key for app,\n(I\'ts kind of password that you will need while signing in!)\n')

            passwordGetterForCreateAnAccount = input('[?] KEY : ')
            while passwordGetterForCreateAnAccount.replace(' ', '') == '':
                self.Funcs.clear_terminal
                self.Funcs.print_first_message_line_on_top('SIGNUP')
                print(
                    'Enter new key for app,\n(I\'ts kind of password that you will need while signing in!)\n\n[!] Don\'t leave key empty.\n')
                passwordGetterForCreateAnAccount = input('[?] KEY : ')
            else:
                print('SETTING THE KEY UP ...', end='\r')
                self.Registers.set_reg('USBPanelKey', passwordGetterForCreateAnAccount)
                print('SETTING THE KEY UP [DONE]')

    def login(self):
        ACCESS = 'F'
        ORIGINAL_PASSWORD = self.Registers.get_reg('USBPanelKey')
        if ORIGINAL_PASSWORD is None:
            self.register()
        else:
            self.Funcs.clear_terminal
            self.Funcs.print_first_message_line_on_top('ACCESS')
            print('Enter the key that you registered with,\n')
            password_getter = str(input('[*] KEY : '))
            wrong_times = 0
            while password_getter != ORIGINAL_PASSWORD:
                wrong_times += 1
                self.Funcs.clear_terminal
                self.Funcs.print_first_message_line_on_top('ACCESS')
                print('Enter the key that you registered with,\n\n[!] WRONG KEY')

                self.wrong_time_password_sleep(wrong_times)

                password_getter = str(input('[*] KEY : '))
            else:
                ACCESS = 'T'
        return ACCESS

    # ---------------------- MORE-START ----------------------
    @property
    def get_app_state(self):
        app_state_content = self.getFileContent(fr'C:\Users\{getuser()}\AppData\Roaming\off_on_state.txt')
        if app_state_content.replace(' ', '') == 'NO':
            return 'OFFLINE'
        else:
            return 'ONLINE'

    @property
    def turn_off_app(self):
        self.write_on_file(fr'C:\Users\{getuser()}\AppData\Roaming\off_on_state.txt', 'NO')

    @property
    def turn_on_app(self):
        self.write_on_file(fr'C:\Users\{getuser()}\AppData\Roaming\off_on_state.txt', 'YES')

    @property
    def get_all_trusted_device(self):
        return literal_eval(self.getFileContent(fr'C:\Users\{getuser()}\AppData\Roaming\trustedDevices.txt'))

    @property
    def print_app_state(self):
        print(f'[STATE] : {self.get_app_state}')

    def replace_all(self, text, dic):
        for i, j in dic.items():
            text = text.replace(i, j)
        return text

    @property
    def find_all_available_device(self):
        filter_names = {
            '\n': '',
            '$0L1F30$': "'",
            '\\': '\\\\'
        }
        all_devs = {}
        [all_devs.update(literal_eval(self.replace_all(data, filter_names))) for data in os.popen(
            '''powershell -command "$hash = $null;$hash = @{};
            $usb_dev = gwmi win32_volume |
            where{$_.DriveType -eq '5' -or $_.DriveType -eq '3' -or $_.DriveType -eq '2'};
            $damn = 0;foreach ($p in $usb_dev)
            {$finallText = '{' + '$0L1F30${0}$0L1F30$:' -f $p.DeviceID +
            '{'+'$0L1F30$SerialNumber$0L1F30$ : $0L1F30${0}$0L1F30$,
            $0L1F30$Name$0L1F30$: $0L1F30${1}$0L1F30$ ,$0L1F30$Label$0L1F30$ : $0L1F30${2}$0L1F30$,
            $0L1F30$DriveType$0L1F30$: $0L1F30${3}$0L1F30$, $0L1F30$DriveLetter$0L1F30$: $0L1F30${4}$0L1F30$,
            $0L1F30$Caption$0L1F30$: $0L1F30${5}$0L1F30$, $0L1F30$Capacity$0L1F30$: $0L1F30${6}$0L1F30$,
            $0L1F30$FreeSpace$0L1F30$: $0L1F30${7}$0L1F30$, $0L1F30$FileSystem$0L1F30$: $0L1F30${8}$0L1F30$,
            $0L1F30$BlockSize$0L1F30$: $0L1F30${9}$0L1F30$' -f $p.SerialNumber, $p.Name, $p.Label, $p.DriveType,
            $p.DriveLetter, $p.Caption, $p.Capacity, $p.FreeSpace, $p.FileSystem, $p.BlockSize + '}' + '}';
            $hash.add($damn, $finallText); $damn+=1};$damn=0;$all_data = $hash | Select-Object -Property *;
            echo $all_data.Values;"''').readlines()]

        return all_devs

    def findNewAndUntrustedConnectedDevicesForAddToList(self, last_device_list):
        all_trusted_devices = self.get_all_trusted_device
        last_founded_devices = last_device_list
        all_available_device = self.find_all_available_device

        new_devices = {}
        for available_dev in all_available_device:
            if available_dev not in all_trusted_devices and available_dev not in last_founded_devices:
                new_devices.update(all_available_device[available_dev])
                new_devices.update({'DeviceID': available_dev})
                break
        return new_devices

    def check_file_existence(self, FileName):
        try:
            return os.path.exists(FileName)
        except:
            pass

    def findNewAndUntrustedConnectedDevicesForFixDevice(self, last_device_list):
        all_trusted_devices = self.get_all_trusted_device
        last_founded_devices = last_device_list
        all_available_device = self.find_all_available_device

        new_devices = {}
        for available_dev in all_available_device:
            if available_dev not in all_trusted_devices and available_dev not in last_founded_devices and \
                    all_available_device[available_dev]['DriveLetter'] == '':
                new_devices.update(all_available_device[available_dev])
                new_devices.update({'DeviceID': available_dev})
                break
        return new_devices

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

    # ---------------------- MORE-END ----------------------

    def control_panel_launch(self, access):
        if access == 'T':
            while True:
                self.Funcs.clear_terminal
                self.Funcs.print_first_message_line_on_top('MENU')
                self.print_app_state
                print('\n[1] DEVICES\n[2] PREFERENCES\n[3] RELOAD\n[0] EXIT\n')

                mod_selector_menu = str(input('admin@admin~$ ')).replace(' ', '')

                while mod_selector_menu not in ['1', '2', '3', '0']:
                    self.Funcs.clear_terminal
                    self.Funcs.print_first_message_line_on_top('MENU')
                    self.print_app_state
                    print('\n[1] DEVICES\n[2] PREFERENCES\n[3] RELOAD\n[0] EXIT\n')
                    mod_selector_menu = str(input('admin@admin~$ ')).replace(' ', '')

                if mod_selector_menu == '1':
                    while True:
                        self.Funcs.clear_terminal
                        self.Funcs.print_first_message_line_on_top('DEVICES')
                        self.print_app_state
                        print(
                            '\n[1] TRUSTED DEVICES\n[2] ADD DEVICE TO TRUSTED DEVICE LIST\n[3] REMOVE DEVICE FROM TRUSTED DEVICE LIST\n[4] LOGS\n[5] FIX KILLED DEVICE\n[0] BACK\n')
                        mod_selector_devices = str(input('(root>devices) admin@admin~$ ')).replace(' ', '')
                        while mod_selector_devices not in ['1', '2', '3', '4', '5', '0']:
                            self.Funcs.clear_terminal
                            self.Funcs.print_first_message_line_on_top('DEVICES')
                            self.print_app_state
                            print(
                                '\n[1] TRUSTED DEVICES\n[2] ADD DEVICE TO TRUSTED DEVICE LIST\n[3] REMOVE DEVICE FROM TRUSTED DEVICE LIST\n[4] LOGS\n[5] FIX KILLED DEVICE\n[0] BACK\n')
                            mod_selector_devices = str(input('(root>devices) admin@admin~$ ')).replace(' ', '')
                        else:
                            if mod_selector_devices == '1':  # list trusted devices
                                self.Funcs.clear_terminal
                                self.Funcs.print_first_message_line_on_top('TRUSTED-DEVS')
                                all_trusted_devices = self.get_all_trusted_device
                                t_d_n = 1
                                for trusted_dev_id, trusted_available_dev_data in all_trusted_devices.items():
                                    if str(trusted_available_dev_data["DriveType"]) == '2':
                                        category = 'Removable storages(Like: USB, External Derives and etc.)'
                                    elif str(trusted_available_dev_data["DriveType"]) == '3':
                                        category = 'Internal HardDisks(Like: HDD, SSD, M2 and etc.)'
                                    elif str(trusted_available_dev_data["DriveType"]) == '5':
                                        category = 'Optical Disc Device(Like: CD, DVD-ROM and etc.)'
                                    else:
                                        category = 'Unknown'
                                    print(
                                        f'--{t_d_n} [Name: {trusted_available_dev_data["Name"]}]
                                        [SerialNumber: {trusted_available_dev_data["SerialNumber"]}]
                                        [Capacity: {self.convert_bytes(trusted_available_dev_data["Capacity"])}]
                                        [FreeSpace: {self.convert_bytes(trusted_available_dev_data["FreeSpace"])}]
                                        [FormatType: {trusted_available_dev_data["FileSystem"]}]
                                        [Category: {category}]')
                                    t_d_n += 1
                                self.Funcs.press_enter_to_continue(True)
                            elif mod_selector_devices == '2':  # add dev to trusted list
                                self.turn_off_app
                                self.Funcs.clear_terminal
                                self.Funcs.print_first_message_line_on_top('SCANNING')
                                time.sleep(2)
                                last_founded_devs = self.find_all_available_device
                                new_founded_device_to_add = self.findNewAndUntrustedConnectedDevicesForAddToList(
                                    last_founded_devs)
                                stopper = 0
                                while len(new_founded_device_to_add) < 1:
                                    self.Funcs.clear_terminal
                                    self.Funcs.print_first_message_line_on_top('SCANNING')
                                    print('CONNECT YOUR DEVICE ...\n')
                                    print(f'[*] SEARCHING FOR NEW CONNECTED DEVICES ENDS IN {50 - stopper}', end='\r')
                                    new_founded_device_to_add = self.findNewAndUntrustedConnectedDevicesForAddToList(
                                        last_founded_devs)

                                    if stopper >= 50:
                                        print('')
                                        what_to_do_while_scanning = str(input('Retry scan ?[y,n] '))
                                        stopper = 0
                                        while what_to_do_while_scanning.lower() not in ['yes', 'yeah', 'yea', 'y', 't',
                                                                                        'true', 'k', 'ok', 'no', 'noh',
                                                                                        'nah', 'noo', 'n', 'f',
                                                                                        'false']:

                                            what_to_do_while_scanning = str(input('Continue scan [y,n]?'))
                                        else:
                                            if what_to_do_while_scanning.lower() in ['yes', 'yeah', 'yea', 'y', 't',
                                                                                     'true', 'k', 'ok', ]:
                                                pass
                                            elif what_to_do_while_scanning.lower() in ['no', 'noh', 'nah', 'noo', 'n',
                                                                                       'f', 'false']:
                                                self.turn_on_app
                                                break
                                    stopper += 1
                                else:
                                    print('')
                                    self.Funcs.clear_terminal
                                    self.Funcs.print_first_message_line_on_top('SCANNING')
                                    print('Device founded!\n')
                                    if str(new_founded_device_to_add["DriveType"]) == '2':
                                        category = 'Removable storages(Like: USB, External Derives and etc.)'
                                    elif str(new_founded_device_to_add["DriveType"]) == '3':
                                        category = 'Internal HardDisks(Like: HDD, SSD, M2 and etc.)'
                                    elif str(new_founded_device_to_add["DriveType"]) == '5':
                                        category = 'Optical Disc Device(Like: CD, DVD-ROM and etc.)'
                                    else:
                                        category = 'Unknown'

                                    packet_for_add_dev = f'[*] Name : {new_founded_device_to_add["Name"]}\n[*] Label : {new_founded_device_to_add["Label"]}\n[*] Capacity : {self.convert_bytes(new_founded_device_to_add["Capacity"])}\n[*] FreeSpace : {self.convert_bytes(new_founded_device_to_add["FreeSpace"])}\n[*] FormatType : {new_founded_device_to_add["FileSystem"]}\n[*] FormatBlockSize : {new_founded_device_to_add["BlockSize"]}\n[*] Category : {category}'

                                    print(packet_for_add_dev)

                                    print('\nAre you sure to add this device to trusted device list ?')
                                    access_to_add_dev_to_trusted_list = str(input('[!] ARE YOU SURE (y,n): '))

                                    while access_to_add_dev_to_trusted_list.lower() not in ['yes', 'yah', 'yeah', 'yea',
                                                                                            'y',
                                                                                            't',
                                                                                            'true', 'k', 'ok', 'no',
                                                                                            'noh',
                                                                                            'nah', 'noo', 'n', 'f',
                                                                                            'false']:
                                        self.Funcs.clear_terminal
                                        self.Funcs.print_first_message_line_on_top('SCANNING')
                                        print(packet_for_add_dev)
                                        print('\nAre you sure to add this device to trusted device list ?')
                                        access_to_add_dev_to_trusted_list = str(input('[!] ARE YOU SURE (y,n): '))
                                    else:
                                        print('[*] Adding selected device to list ...', end='\r')
                                        if access_to_add_dev_to_trusted_list.lower() in ['yes', 'yah', 'yeah', 'yea',
                                                                                         'y', 't',
                                                                                         'true', 'k', 'ok']:
                                            all_tr_devs = self.get_all_trusted_device

                                            all_tr_devs.update({new_founded_device_to_add['DeviceID']: {
                                                'SerialNumber': new_founded_device_to_add['SerialNumber'],
                                                'Name': new_founded_device_to_add['Name'],
                                                'Label': new_founded_device_to_add['Label'],
                                                'DriveType': new_founded_device_to_add['DriveType'],
                                                'DriveLetter': new_founded_device_to_add['DriveLetter'],
                                                'Caption': new_founded_device_to_add['Caption'],
                                                'Capacity': new_founded_device_to_add['Capacity'],
                                                'FreeSpace': new_founded_device_to_add['FreeSpace'],
                                                'FileSystem': new_founded_device_to_add['FileSystem'],
                                                'BlockSize': new_founded_device_to_add['BlockSize']}})
                                            self.write_on_file(
                                                fr'C:\Users\{getuser()}\AppData\Roaming\trustedDevices.txt',
                                                all_tr_devs)
                                            print('[+] Adding selected device to list [DONE]')
                                            self.Funcs.press_enter_to_continue(True,
                                                                               'Press enter to going back to menu')
                                            self.turn_on_app
                                            break

                                        elif access_to_add_dev_to_trusted_list.lower() in ['no', 'noh', 'nah', 'noo',
                                                                                           'n',
                                                                                           'f', 'false']:
                                            self.turn_on_app
                                            break
                                self.turn_on_app
                            elif mod_selector_devices == '3':  # del device from trusted list
                                self.Funcs.clear_terminal
                                self.Funcs.print_first_message_line_on_top('DELETE-DEV')
                                all_trusted_devices_for_del = self.get_all_trusted_device
                                dev_dict_for_del = {}
                                del_dev_num = 1
                                for trusted_dev_id_for_del, trusted_dev_value_for_del in all_trusted_devices_for_del.items():
                                    if str(trusted_dev_value_for_del["DriveType"]) == '2':
                                        category = 'Removable storages(Like: USB, External Derives and etc.)'
                                    elif str(trusted_dev_value_for_del["DriveType"]) == '3':
                                        category = 'Internal HardDisks(Like: HDD, SSD, M2 and etc.)'
                                    elif str(trusted_dev_value_for_del["DriveType"]) == '5':
                                        category = 'Optical Disc Device(Like: CD, DVD-ROM and etc.)'
                                    else:
                                        category = 'Unknown'

                                    print(
                                        f'--{del_dev_num} [Name: {trusted_dev_value_for_del["Name"]}][SerialNumber: {trusted_dev_value_for_del["SerialNumber"]}][Capacity: {self.convert_bytes(trusted_dev_value_for_del["Capacity"])}][FreeSpace: {self.convert_bytes(trusted_dev_value_for_del["FreeSpace"])}][FormatType: {trusted_dev_value_for_del["FileSystem"]}][Category: {category}]')
                                    dev_dict_for_del.update({f'{del_dev_num}': trusted_dev_id_for_del})
                                    del_dev_num += 1
                                print('\n\n\033[FEnter the device id, like 1, 2, etc, (0 to exit).')

                                what_to_do_for_del_dev = str(input('[?] DeviceID : '))
                                if what_to_do_for_del_dev == '0':
                                    break
                                while dev_dict_for_del.get(what_to_do_for_del_dev, 'NOT FOUND') == 'NOT FOUND':
                                    print(
                                        f'\033[F\033[FSelected device "{what_to_do_for_del_dev}" doesnt exists.{" " * 50}')
                                    print(f'{" " * 50}')
                                    what_to_do_for_del_dev = str(input('\033[F[?] DeviceID : '))
                                    if what_to_do_for_del_dev == '0':
                                        break
                                else:
                                    get_access_for_del = str(input('\n[?] ARE YOU SURE (y,n) ? '))
                                    while get_access_for_del.lower() not in ['yes', 'yah', 'yeah', 'yea',
                                                                             'y',
                                                                             't',
                                                                             'true', 'k', 'ok', 'no',
                                                                             'noh',
                                                                             'nah', 'noo', 'n', 'f',
                                                                             'false']:
                                        print(
                                            f'\033[F\033[FEnter something between [y,n], not "{get_access_for_del}"{" " * 50}')
                                        print(f'{" " * 80}')
                                        get_access_for_del = str(input('\033[F[?] ARE YOU SURE (y,n) ? '))
                                    else:
                                        if get_access_for_del.lower() in ['yes', 'yah', 'yeah', 'yea',
                                                                          'y', 't',
                                                                          'true', 'k', 'ok']:
                                            all_trusted_devices_for_del.pop(
                                                dev_dict_for_del.get(what_to_do_for_del_dev))

                                            self.write_on_file(
                                                fr'C:\Users\{getuser()}\AppData\Roaming\trustedDevices.txt',
                                                all_trusted_devices_for_del)

                                            self.Funcs.press_enter_to_continue(True,
                                                                               'Press enter to going back to menu')
                                        elif get_access_for_del.lower() in ['no', 'noh', 'nah', 'noo',
                                                                            'n',
                                                                            'f', 'false']:
                                            break
                            elif mod_selector_devices == '4':
                                self.Funcs.clear_terminal
                                self.Funcs.print_first_message_line_on_top('LOGS')
                                devs_logs = self.getFileContent(
                                    fr'C:\Users\{getuser()}\AppData\Roaming\untrustedDeviceLogs.txt').splitlines()

                                log_num = 1
                                for dev_log in devs_logs:
                                    print(f'-- {log_num} {dev_log}\n')
                                    log_num += 1
                                self.Funcs.press_enter_to_continue(True, 'Press enter to going back to menu')
                            elif mod_selector_devices == '5':
                                self.turn_off_app
                                self.Funcs.clear_terminal
                                self.Funcs.print_first_message_line_on_top('SCANNING')
                                time.sleep(2)
                                last_founded_devs = self.find_all_available_device
                                new_founded_device_to_add = self.findNewAndUntrustedConnectedDevicesForFixDevice(
                                    last_founded_devs)
                                stopper = 0
                                while len(new_founded_device_to_add) < 1:
                                    self.Funcs.clear_terminal
                                    self.Funcs.print_first_message_line_on_top('SCANNING')
                                    print('CONNECT YOUR DEVICE ...\n')
                                    print(f'[*] SEARCHING FOR NEW CONNECTED DEVICES ENDS IN {50 - stopper}', end='\r')
                                    new_founded_device_to_add = self.findNewAndUntrustedConnectedDevicesForFixDevice(
                                        last_founded_devs)

                                    if stopper >= 50:
                                        print('')
                                        what_to_do_while_scanning = str(input('Retry scan ?[y,n] '))
                                        stopper = 0
                                        while what_to_do_while_scanning.lower() not in ['yes', 'yeah', 'yea', 'y', 't',
                                                                                        'true', 'k', 'ok', 'no', 'noh',
                                                                                        'nah', 'noo', 'n', 'f',
                                                                                        'false']:

                                            what_to_do_while_scanning = str(input('Retry scan [y,n]?'))
                                        else:
                                            if what_to_do_while_scanning.lower() in ['yes', 'yeah', 'yea', 'y', 't',
                                                                                     'true', 'k', 'ok', ]:
                                                pass
                                            elif what_to_do_while_scanning.lower() in ['no', 'noh', 'nah', 'noo', 'n',
                                                                                       'f', 'false']:
                                                self.turn_on_app
                                                break
                                    stopper += 1
                                else:
                                    print('')
                                    self.Funcs.clear_terminal
                                    self.Funcs.print_first_message_line_on_top('SCANNING')
                                    print('Device founded!\n')
                                    if str(new_founded_device_to_add["DriveType"]) == '2':
                                        category = 'Removable storages(Like: USB, External Derives and etc.)'
                                    elif str(new_founded_device_to_add["DriveType"]) == '3':
                                        category = 'Internal HardDisks(Like: HDD, SSD, M2 and etc.)'
                                    elif str(new_founded_device_to_add["DriveType"]) == '5':
                                        category = 'Optical Disc Device(Like: CD, DVD-ROM and etc.)'
                                    else:
                                        category = 'Unknown'

                                    packet_for_add_dev = f'[*] Name : {new_founded_device_to_add["Name"]}\n
                                    [*] Label : {new_founded_device_to_add["Label"]}\n
                                    [*] Capacity : {self.convert_bytes(new_founded_device_to_add["Capacity"])}\n
                                    [*] FreeSpace : {self.convert_bytes(new_founded_device_to_add["FreeSpace"])}\n
                                    [*] FormatType : {new_founded_device_to_add["FileSystem"]}\n
                                    [*] FormatBlockSize : {new_founded_device_to_add["BlockSize"]}\n
                                    [*] Category : {category}'

                                    print(packet_for_add_dev)

                                    access_to_add_dev_to_trusted_list = str(input('[!] ARE YOU SURE (y,n): '))

                                    while access_to_add_dev_to_trusted_list.lower() not in ['yes', 'yah', 'yeah', 'yea',
                                                                                            'y',
                                                                                            't',
                                                                                            'true', 'k', 'ok', 'no',
                                                                                            'noh',
                                                                                            'nah', 'noo', 'n', 'f',
                                                                                            'false']:
                                        self.Funcs.clear_terminal
                                        self.Funcs.print_first_message_line_on_top('SCANNING')
                                        print(packet_for_add_dev)
                                        access_to_add_dev_to_trusted_list = str(input('[!] ARE YOU SURE (y,n): '))
                                    else:
                                        if access_to_add_dev_to_trusted_list.lower() in ['yes', 'yah', 'yeah', 'yea',
                                                                                         'y', 't',
                                                                                         'true', 'k', 'ok']:
                                            print('[*] Repairing selected device ...', end='\r')

                                            command_state = os.popen(
                                                r'powershell -command "$usbdev = gwmi win32_volume | where{$_.DeviceID -eq ' + f'\'{new_founded_device_to_add["DeviceID"]}\'' + '};$usbdev.DriveLetter = ' + f'\'Z:\';$usbdev.Put()"').read()

                                            index_except = 0
                                            while 'Exception calling' in command_state:
                                                if index_except > 25:
                                                    print('SOMETHING WENT WRONG!')
                                                    time.sleep(5)
                                                    break
                                                else:
                                                    command_state = os.popen(
                                                        r'powershell -command "$usbdev = gwmi win32_volume
                                                        | where{$_.DeviceID -eq '
                                                        + f'\'{new_founded_device_to_add["DeviceID"]}\''
                                                        + '};$usbdev.DriveLetter = '
                                                        + f'\'{str(string.ascii_uppercase)[index_except]}:\';
                                                        $usbdev.Put()"').read()
                                                index_except += 1

                                            print('[+] Repairing selected device [DONE]')
                                            self.Funcs.press_enter_to_continue(True,
                                                                               '[!] NOTICE : REMOVE YOUR FLASH AND THEN PRESS THE ENTER!')
                                            self.turn_on_app
                                            break
                            elif mod_selector_devices == '0':
                                break
                            else:
                                print('\nSELECTED MOD NOT FOUNDED')
                                time.sleep(1)
                elif mod_selector_menu == '2':
                    while True:
                        self.Funcs.clear_terminal
                        self.Funcs.print_first_message_line_on_top('LOGS')

                        self.print_app_state

                        print(
                            '\n[1] TURN THE APP OFF\n[2] TURN THE APP ON\n[3] CHANGE KEY\n[4] CLEAR DATA\n[0] BACK\n')

                        command_getter_on_preference = str(input('(root>preferences) admin@admin~$ '))
                        while command_getter_on_preference not in ['1', '2', '3', '4', '0']:
                            self.Funcs.clear_terminal
                            self.Funcs.print_first_message_line_on_top('LOGS')

                            print(
                                '[1] TURN THE APP OFF\n[2] TURN THE APP ON\n[3] CHANGE KEY\n[4] CLEAR DATA\n[0] BACK\n')

                            command_getter_on_preference = str(input('(root>preferences) admin@admin~$ '))
                        else:
                            if command_getter_on_preference == '1':
                                self.turn_off_app
                            elif command_getter_on_preference == '2':
                                self.turn_on_app
                            elif command_getter_on_preference == '3':
                                self.Funcs.clear_terminal
                                self.Funcs.print_first_message_line_on_top('CHANGE-KEY')
                                ORG_KEY = self.Registers.get_reg('USBPanelKey')
                                org_key_getter = str(input('[*] CURRENT KEY : '))
                                wrong_tms = 0
                                while org_key_getter != ORG_KEY:
                                    self.Funcs.clear_terminal
                                    self.Funcs.print_first_message_line_on_top('CHANGE-KEY')
                                    self.wrong_time_password_sleep(wrong_tms)
                                    wrong_tms += 1
                                    org_key_getter = str(input('[*] CURRENT KEY : '))
                                else:
                                    self.Funcs.clear_terminal
                                    self.Funcs.print_first_message_line_on_top('CHANGE-KEY')
                                    print(f'[+] CURRENT KEY : {org_key_getter}')

                                    new_key_getter = str(input('[*] NEW KEY : '))
                                    while new_key_getter.replace(' ', '') == '':
                                        self.Funcs.clear_terminal
                                        self.Funcs.print_first_message_line_on_top('CHANGE-KEY')
                                        print(f'[+] CURRENT KEY : {org_key_getter}')
                                        new_key_getter = str(input('[*] NEW KEY : '))
                                    else:
                                        self.Registers.set_reg('USBPanelKey', new_key_getter)
                            elif command_getter_on_preference == '4':
                                try:
                                    os.remove(fr'C:\Users\{getuser()}\AppData\Roaming\trustedDevices.txt')
                                    print('[!] DATA CLEARED.')
                                    time.sleep(2)
                                except:
                                    print('[!] DATA ALREADY CLEARED.')
                                    time.sleep(2)
                            elif command_getter_on_preference == '0':
                                break
                elif mod_selector_menu == '3':
                    pass
                elif mod_selector_menu == '0':
                    quit()
                else:
                    print('\nSELECTED MOD NOT FOUNDED')
                    time.sleep(1)
        else:
            print('\nACCESS DENIED,\nOpen the app again and try to login.')
            self.Funcs.press_enter_to_continue()


ControlPanel = Panel()
if ControlPanel.Registers.get_reg('DoesUSBProtectorInstalled') == 'YES' or ControlPanel.check_file_existence(
        fr'C:\Users\{getuser()}\AppData\Roaming\trustedDevices.txt'):
    ControlPanel.control_panel_launch(ControlPanel.login())
else:
    print('FIRST INSTALL THE APP')
    ControlPanel.Funcs.press_enter_to_continue(True)
