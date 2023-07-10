# PlugProtector
PlugProtector is a security project that safeguards a system's USB ports from untrusted devices


> ## **Supported on**:
> - [x] Windows
> - [ ] Linux <sup>&#8226; maybe someday...</sup>


The PlugProtector is a security project implemented in **C** and **C++** languages.
It provides robust protection for a system's USB ports against untrusted USB devices. It allows users to create a **Trusted Device List**, and any USB devices that are not part of the list, immediately gets ejected upon connection.

In addition to the protective ejection mechanism, the PlugProtector also features a **Destructive Mode**. When activated, this mode intentionally corrupts any connected USB drive irreversibly/reversibly, making it unusable. The destructive is optional, you can turn it off if you want.

This project ensures the security and integrity of the system by allowing users to control which USB devices can connect to it. With that being said, users have the peace of mind that untrusted devices are automatically rejected or corrupted.


## **Program Modes:**
1. **Destructive Mode** <sup>&#8226; OPTIONAL</sup>
   - Has 2 Modes
     - `Irreversible:` As it names says, it will corrupt few bytes (file system and headers) of the connected device if possible. then ejects the device.
     - `Reversible:` Unlike the **Irreversible** mode, it will store the corrupted bytes for unintentional device destruction, meaning that you can restore the destructed device later from the **Control Panel**
2. **Eject Mode**
   - Has Only 1 Mode
     - `Reversible:` Ejects and removes the name of the device, so it wont show up the next time you connect the usb to a system.
