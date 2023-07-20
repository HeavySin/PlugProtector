from getpass import getuser
from msvcrt import getch
from shutil import copy
import os
import sys
import time
import winreg
import winshell

REG_PATH = r"Control Panel\Keyboard"


def multiplication(src='', dest=''):
    try:
        current_file_path = str(src)
        destination_file_path = str(dest)
        copy(current_file_path, destination_file_path)
    except:
        pass


def set_reg(name, value, path=REG_PATH):
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


def get_reg(name, path=REG_PATH):
    try:
        registry_key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, path, 0,
                                      winreg.KEY_READ)
        value, regtype = winreg.QueryValueEx(registry_key, name)
        winreg.CloseKey(registry_key)
        return value
    except WindowsError:
        return None


def clear_terminal():
    os.system('cls' if os.name == 'nt' else 'clear')


def press_enter_to_continue(show_text=False, text=None):
    if show_text and text:
        print(f'\n{text}')
    elif show_text:
        print('\nPress enter to continue')
    getch()


def print_first_message_line_on_top(message=''):
    print(f'------------- {message.upper()} -------------\n\n')


def verify_dependencies():
    clear_terminal()
    print_first_message_line_on_top('PROCESSING')
    print('[!] Verifying dependencies ...', end='\r')
    exe_files = [f for f in os.listdir('./') if f.endswith('.exe')]
    if 'usbProtection.exe' not in exe_files:
        clear_terminal()
        print_first_message_line_on_top('DEPENDENCIES-NOT-FOUND')
        print('[!] "usbProtection.exe" not found')
        press_enter_to_continue(True, 'FIX IT AND TRY AGAIN')
    else:
        if 'RemoveDrive.exe' not in exe_files:
            clear_terminal()
            print_first_message_line_on_top('DEPENDENCIES-NOT-FOUND')
            print('[!] "RemoveDrive.exe" not found')
            press_enter_to_continue(True, 'FIX IT AND TRY AGAIN')
        else:
            time.sleep(1)
            print('[+] Verifying dependencies [DONE]')
            return True


if verify_dependencies():
    clear_terminal()
    print_first_message_line_on_top('LAUNCHER')
    print('[!] Launching ...', end='\r')
    if get_reg('DoesUSBLauncherUsed') != 'YES':
        multiplication('./RemoveDrive.exe', fr'C:\Users\{getuser()}\AppData\Roaming')
        time.sleep(5)
        os.popen('usbProtection.exe')
        set_reg('DoesUSBLauncherUsed', 'YES')
        print('[!] Launching [DONE]')
        press_enter_to_continue(True)
    else:
        clear_terminal()
        print_first_message_line_on_top('POLICIES')
        print('LAUNCHER USED ONCE BEFORE,')
        for _ in range(6):
            print(f'Exit in {6 - _} seconds.', end='\r')
            time.sleep(1)
