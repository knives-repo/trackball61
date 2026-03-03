MCKWorkshop：<br/>
trackballseries firmware<br/>
本系列layout参考了:[keyball](https://github.com/Yowkees/keyball)<br/>
firmware代码是基于[Charybdis](https://bastardkb.com)的源码进行二次开发，增加了一些实用性功能<br/>
后缀为ctc的就是双type c连接的版本，不带ctc的为trs连接版本<br/>
如果你的键盘名称后缀带V2/V3使用的传感器是PMW3389传感器，如果不带后缀则使用的是PMW3360传感器<br/>
如果想编译自己的固件，只需要根据键盘后缀判断自己的传感器，然后调整各配列下v2/splinky/rules.mk中的POINTING_DEVICE_DRIVER即可<br/>

MCKWorkshop:<br/>
Trackballseries firmware<br/>
This series layout references: [keyball](https://github.com/Yowkees/keyball)<br/>
The firmware code is a secondary development based on the source code of [Charybdis](https://bastardkb.com), adding some practical functions.<br/> Versions with the suffix ".ctc" are dual Type-C connection versions, while those without ".ctc" are TRS connection versions.<br/>
If your keyboard name suffix includes "V2/V3", it uses the PMW3389 sensor; otherwise, it uses the PMW3360 sensor.<br/>
To compile your own firmware, simply determine your sensor based on the keyboard suffix and then adjust the "POINTING_DEVICE_DRIVER" in v2/splinky/rules.mk for each layout.<br/>
