# kwatchdog_kmod
FreeBSD kernel-side watchdog sample

Let's make cpu_reset if module not unloaded in time. Default timeout = 60 seconds.

Usage:

In /boot/loader.conf:

~~~
kwatchdog_load="YES"
~~~


Tunable, max counter:

~~~
kwatchdog.count.critical=50
~~~

Execute the kldunload in your script before kwatchdog.count.critical ends

P.s: Watch themessages in dmesg
