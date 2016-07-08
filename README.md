# SA-MP Masterlist Fix
Fixes the infamous SA-MP internet tab bug.



## Installation:
* Download the latest package from the [releases page](https://github.com/spmn/sa-mp_masterlist_fix/releases);
* Unpack it into SA-MP directory where `samp.exe` is located;
* Run `masterlist_fix.exe`


## How does it work?
Simple! 

This fix injects a DLL into SA-MP client which redirects all the requests outgoing to SA-MP Masterlist (hosted list is not affected) to the server specified in `masterlist_fix.cfg` (default: [SACNR Monitor](http://monitor.sacnr.com/))

Why SACNR? Because they have implemented an API which allows adding of new servers to their Monitor by simple running a filterscript. Details [here](http://monitor.sacnr.com/api.html). 


## My server still is not appearing on the internet tab.
Most probably your server is not registered on the [SACNR Monitor](http://monitor.sacnr.com/). 

You can register your server by following these [steps](http://monitor.sacnr.com/api.html). In the next hours the server should appear on internet tab.


## How can I contribute?
An **auto injector** would make this project more user-friendly because (at this moment) the DLL is injected into SA-MP process by a helper executable `masterlist_fix.exe` which means that the user has to open THAT executable instead of the original executable `samp.exe`.
