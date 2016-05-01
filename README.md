# SA-MP Masterlist Fix
Fixes the infamous SA-MP internet tab bug.



## Installation:
* Download the latest package from the [releases page](https://github.com/spmn/sa-mp_masterlist_fix/releases);
* Unpack it into SA-MP directory where `samp.exe` is located;
* Run `masterlist_fix.exe`


## How does it work?
Simple! This fix is composed of two main parts:

1. A php script which parses [SACNR Monitor](http://monitor.sacnr.com/) two times a day and saves all server IPs on my (free) webhost server in the format used by SA-MP client;

 Why SACNR? Because they have implemented an API which allows adding of new servers to their Monitor by simple running a filterscript. Details [here](http://monitor.sacnr.com/api.html). 

2. A DLL injected into SA-MP client which redirects all the traffic outgoing to SA-MP Masterlist to the server specified in `masterlist_fix.cfg` (default: my webhost server)


## My server still is not appearing on the internet tab.
Most probably your server is not registered on the [SACNR Monitor](http://monitor.sacnr.com/). 

You can register your server by following these [steps](http://monitor.sacnr.com/api.html). In the next ~12 hours the server should appear on internet tab.


## How can I contribute?
There are several parts in this fix which can get better:

1. Masterlist server
  * this is currently hosted on a free account, which means:
    * it may or may not handle heavy traffic;
    * the account can be terminated at any time.
  * if you have a webhost/VPS/dedi with unlimited bandwidth and relatively high transfer speed at your disposal then you can help this project by hosting the masterlist. Just upload `update_masterlist.php` to your server, `crontab` it and create an issue containing the link to `masterlist.txt`;
  * or better, if you're a maintainer of SACNR Monitor API and do not mind high traffic, then you can help by directly providing a list of servers in SA-MP client format, so the parser will be no longer needed.

2. Injection code
  * this fix is currently injected into SA-MP process by a helper executable `masterlist_fix.exe` which means that the user has to open THAT executable instead of the original executable `samp.exe`
  * an auto injector (like ASI Loader) would be a perfect solution

