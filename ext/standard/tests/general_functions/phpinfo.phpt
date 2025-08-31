--TEST--
phpinfo()
--FILE--
<?php
var_dump(phpinfo());

echo "--\n";
var_dump(phpinfo(0));

echo "--\n";
var_dump(phpinfo(INFO_LICENSE));

?>
--EXPECTF--
phpinfo()
PHP Version => %s

System => %s
Build Date => %r(.+?)%r
Configure Command => %s
Server API => Command Line Interface
Virtual Directory Support => %s
Configuration File (php.ini) Path => %s
Loaded Configuration File => %r(.+?)%r
Scan this dir for additional .ini files => %r(.+?)%r
Additional .ini files parsed => %r(.+?)%r
PHP API => %d
PHP Extension => %d
Zend Extension => %d
Zend Extension Build => API%s
PHP Extension Build => API%s
PHP Integer Size => %d bits
Debug Build => %s
Thread Safety => %r(.+?)%r
Zend Signal Handling => %s
Zend Memory Manager => %s
Zend Multibyte Support => %s
Zend Max Execution Timers => %s
IPv6 Support => %s
DTrace Support => %s

Registered PHP Streams => %s
Registered Stream Socket Transports => %s
Registered Stream Filters => %s

%r(.+?)%r
 _______________________________________________________________________


Configuration
%r(.*?)%r
Core
%r(.*?)%r
Additional Modules
%r(.*?)%r
Environment
%r(.*?)%r
PHP Variables
%r(.*?)%r
PHP License
%r(.*?)%r
bool(true)
--
phpinfo()
bool(true)
--
phpinfo()

PHP License
%r(.+?)%r
bool(true)
