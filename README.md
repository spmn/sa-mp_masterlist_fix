# SA-MP Masterlist Fix
Master of your... internet tab!



## Installation:
* Download the latest `version.dll` from the [releases page](https://github.com/spmn/sa-mp_masterlist_fix/releases/latest);
* Copy `version.dll` to your SA-MP directory (where `samp.exe` is located);
* Run `samp.exe`;
* *Optional:* Tweak the masterlist servers (`My Documents/GTA San Andreas User Files/SAMP/sa-mp_masterlist_fix.json`)


## Compatibility
This mod is confirmed to be compatible with 0.3.7-R1, 0.3.7-R5 and 0.3DL-R1. It should also be compatible with other releases, but that is not guaranteed.

## How does it work?
Simple! 

This fix injects itself into the SA-MP server browser and redirects all masterlist requests to the masterservers specified in `sa-mp_masterlist_fix.json`. (default: [SA Monitor](https://sam.markski.ar/web/about.php))

**New in v2.0:** Now you can change the masterlist used for the hosted tab, too! <br/>
**New in v2.0:** The masterlists can be served from non-default ports.


## My server is not shown on the internet tab.
Most probably your server is not registered on [SA Monitor](https://sam.markski.ar/web/about.php). 

You can register your server by following these [steps](https://sam.markski.ar/web/add.php). In the next hours the server should appear on the internet tab.


## Contributors:
[markski1](https://github.com/markski1) - hosts the masterlist used by default in v2.0.0 <br/>
[SACNR Monitor](http://monitor.sacnr.com/) - hosted the alternative masterlist used for many years by default in v1.1.0 <br/>
[MyU](https://github.com/myudev) - wrote the code for the auto injector used in v1.1.0 <br/>
[nickdodd25](https://github.com/nickdodd25) - hosted the masterlist for v1.0.1 release
