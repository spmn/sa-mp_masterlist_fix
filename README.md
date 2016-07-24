# SA-MP Masterlist Fix
Fixes the infamous SA-MP internet tab bug.



## Installation:
* Download the latest `version.dll` from the [releases page](https://github.com/spmn/sa-mp_masterlist_fix/releases);
* Copy `version.dll` to SA-MP directory where `samp.exe` is located;
* Run `samp.exe`


## How does it work?
Simple! 

This fix injects a DLL into SA-MP client which redirects all the requests outgoing to SA-MP Masterlist (hosted list is not affected) to the server specified in `masterlist_fix.cfg` (default: [SACNR Monitor](http://monitor.sacnr.com/))

Why SACNR? Because they have implemented an API which allows adding of new servers to their Monitor by simple running a filterscript. Details [here](http://monitor.sacnr.com/api.html). 


## My server still is not appearing on the internet tab.
Most probably your server is not registered on the [SACNR Monitor](http://monitor.sacnr.com/). 

You can register your server by following these [steps](http://monitor.sacnr.com/api.html). In the next hours the server should appear on internet tab.


## Contributors:
[MyU](https://github.com/myudev) - wrote the code for the auto injector used in v1.1.0 and subsequent releases
[nickdodd25](https://github.com/nickdodd25) - hosted the masterlist for v1.0.1 release
