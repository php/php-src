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
Build Date => %s%a
Configure Command => %s
Server API => Command Line Interface
Virtual Directory Support => %s
Configuration File (php.ini) Path => %s
Loaded Configuration File => %a
Scan this dir for additional .ini files => %a
Additional .ini files parsed => %a
PHP API => %d
PHP Extension => %d
Zend Extension => %d
Zend Extension Build => API%s
PHP Extension Build => API%s
Debug Build => %s
Thread Safety => %s%A
Zend Memory Manager => %s
Zend Multibyte Support => %s
IPv6 Support => %s
DTrace Support => %s

Registered PHP Streams => %s
Registered Stream Socket Transports => %s
Registered Stream Filters => %s

%a
 _______________________________________________________________________


Configuration
%A
Core
%A
Additional Modules
%A
Environment
%A
PHP Variables
%A
PHP License
%A
bool(true)
--
phpinfo()
bool(true)
--
phpinfo()

PHP License
%a
bool(true)
