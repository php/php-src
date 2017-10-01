Introduction
============

LiteSpeed SAPI module is a dedicated interface for PHP integration with
LiteSpeed Web Server. LiteSpeed SAPI has similar architecture to the
FastCGI SAPI with there major enhancements: better performance, dynamic 
spawning and PHP configuration modification through web server
configuration and .htaccess files.  

Our simple benchmark test ("hello world") shows that PHP with
LiteSpeed SAPI has 30% better performance over PHP with FastCGI SAPI,
which is nearly twice the performance that Apache mod_php can deliver. 
 
A major drawback of FastCGI PHP comparing to Apache mod_php is lacking
the flexibilities in PHP configurations. PHP configurations cannot be 
changed at runtime via configuration files like .htaccess files or web
server's virtual host configuration. In shared hosting environment, 
each hosting account will has its own "open_basedir" overridden in 
server configuration to enhance server security when mod_php is used.
usually, FastCGI PHP is not an option in shared hosting environment 
due to lacking of this flexibility. LiteSpeed SAPI is carefully designed
to address this issue. PHP configurations can be modified the same way 
as that in mod_php with the same configuration directives. 

PHP with LiteSpeed SAPI is highly recommended over FastCGI PHP for 
PHP scripting with LiteSpeed web server. 


Building PHP with LiteSpeed SAPI
================================

You need to add "--with-litespeed" to the configure command to build
PHP with LiteSpeed SAPI, all other SAPI related configure options
should be removed. 

For example: 
    ./configure --with-litespeed
    make

You should find an executable called 'php' under sapi/litespeed/
directory after the compilation succeeds. Copy it to
'lsws/fcgi-bin/lsphp' or wherever you prefer, if LiteSpeed web server
has been configured to run PHP with LiteSpeed SAPI already, you just
need to overwrite the old executable with this one and you are all
set. 

Start PHP from command line
===========================

Usually, lsphp is managed by LiteSpeed web server in a single server
installation. lsphp can be used in clustered environment with one 
LiteSpeed web server at the front, load balancing lsphp processes 
running on multiple backend servers. In such environment, lsphp can be
start manually from command with option "-b <socket_address>", socket 
address can be IPv4, IPv6 or Unix Domain Socket address. 
for example:

    ./lsphp -b [::]:3000

have lsphp bind to port 3000 on all IPv4 and IPv6 address,

    ./lsphp -b *:3000

have lsphp bind to port 300 on all IPv4 address.

    ./lsphp -b 192.168.0.2:3000

have lsphp bind to address 192.168.0.2:3000.

    ./lsphp -b /tmp/lsphp_manual.sock

have lsphp accept request on Unix domain socket "/tmp/lsphp_manual.sock"


Using LiteSpeed PHP with LiteSpeed Web Server
=============================================

Detailed information about how to configure LiteSpeed web server with
PHP support is available from our website, at: 

https://www.litespeedtech.com/docs/webserver

Usually, PHP support has been configured out of box, you don't need to
change it unless you want to change PHP interface from FastCGI to
LiteSpeed SAPI or vice versa. 

Brief instructions are as follow:

1) Login to web administration interface, go to 'Server'->'Ext App' tab,
   add an external application of type "LSAPI app", "Command" should be
   set to a shell command that executes the PHP binary you just built. 
   "Instances" should be set to "1". Add "LSAPI_CHILDREN" environment 
   variable to match the value of "Max Connections". More tunable 
   environment variable described below can be added. 

2) Go to 'Server'->'Script Handler' tab, add a script handler
   configuration: set 'suffix' to 'php', 'Handler Type' to 'LiteSpeed
   API', 'Handler Name' should be the name of external application
   just defined. 


3) Click 'Apply Changes' link on the top left of the page, then click 
   'graceful restart'. Now PHP is running with LiteSpeed SAPI. 

Tunings
-------

There are a few environment variables that can be tweaked to control the
behavior of LSAPI application.  

* LSAPI_CHILDREN or PHP_LSAPI_CHILDREN  (default: 0)

There are two ways to let PHP handle multiple requests concurrently, 
Server Managed Mode and Self Managed Mode. In Server Managed Mode, 
LiteSpeed web server dynamically spawn/stop PHP processes, in this mode
"Instances" should match "Max Connections" configuration for PHP 
external application. To start PHP in Self Managed Mode, "Instances" 
should be set to "1", while "LSAPI_CHILDREN" environment variable should
be set to match the value of "Max Connections" and >1. Web Server will 
start one PHP process, this process will start/stop children PHP processes 
dynamically based on on demand. If "LSAPI_CHILDREN" <=1, PHP will be 
started in server managed mode.

Self Managed Mode is preferred because all PHP processes can share one 
shared memory block for the opcode cache.

Usually, there is no need to set value of LSAPI_CHILDREN over 100 in
most server environment. 


* LSAPI_AVOID_FORK              (default: 0)

LSAPI_AVOID_FORK specifies the policy of the internal process manager in
"Self Managed Mode". When set to 0, the internal process manager will stop
and start children process on demand to save system resource. This is
preferred in a shared hosting environment. When set to 1, the internal
process manager will try to avoid freqently stopping and starting children
process. This might be preferred in a dedicate hosting environment.


* LSAPI_EXTRA_CHILDREN          (default: 1/3 of LSAPI_CHILDREN or 0)

LSAPI_EXTRA_CHILDREN controls the maximum number of extra children processes
can be started when some or all existing children processes are in
malfunctioning state. Total number of children processes will be reduced to
LSAPI_CHILDREN level as soon as service is back to normal.
When LSAPI_AVOID_FORK is set to 0, the default value is 1/3 of
LSAPI_CHIDLREN, When LSAPI_AVOID_FORK is set to 1, the default value is 0.


* LSAPI_MAX_REQS or PHP_LSAPI_MAX_REQUESTS (default value: 10000)

This controls how many requests each child process will handle before
it exits automatically. Several PHP functions have been identified 
having memory leaks. This parameter can help reducing memory usage 
of leaky PHP functions. 


* LSAPI_MAX_IDLE                (default value: 300 seconds)

In Self Managed Mode, LSAPI_MAX_IDLE controls how long a idle child  
process will wait for a new request before it exits. This option help 
releasing system resources taken by idle processes.


* LSAPI_MAX_IDLE_CHILDREN
    (default value: 1/3 of LSAPI_CHILDREN or LSAPI_CHILDREN)

In Self Managed Mode, LSAI_MAX_IDLE_CHILDREN controls how many idle 
children processes are allowed. Excessive idle children processes
will be killed by the parent process immediately.
When LSAPI_AVOID_FORK is set to 0, the default value is 1/3 of
LSAPI_CHIDLREN, When LSAPI_AVOID_FORK is set to 1, the default value
is LSAPI_CHILDREN.


* LSAPI_MAX_PROCESS_TIME        (default value: 300 seconds)

In Self Managed Mode, LSAPI_MAX_PROCESS_TIME controls the maximum 
processing time allowed when processing a request. If a child process
can not finish processing of a request in the given time period, it 
will be killed by the parent process. This option can help getting rid 
of dead or runaway child process.


* LSAPI_PGRP_MAX_IDLE           (default value: FOREVER )

In Self Managed Mode, LSAPI_PGRP_MAX_IDLE controls how long the parent
process will wait before exiting when there is no child process.
This option help releasing system resources taken by an idle parent 
process.


* LSAPI_PPID_NO_CHECK

By default a LSAPI application check the existence of its parent process
and exits automatically if the parent process died. This is to reduce 
orphan process when web server is restarted. However, it is desirable 
to disable this feature, such as when a LSAPI process was started 
manually from command line. LSAPI_PPID_NO_CHECK should be set when 
you want to disable the checking of existence of parent process.
When PHP started by "-b" option, it is disabled automatically. 


Compatibility with Apache mod_php
=================================

LSAPI PHP supports PHP configuration overridden via web server configuration 
as well as .htaccess. 
Since 4.0 release "apache_response_headers" function is supported.



Contact
=======

For support questions, please post to our free support forum, at:

https://www.litespeedtech.com/support/forum/

For bug report, please send bug report to bug [at] litespeedtech.com.




