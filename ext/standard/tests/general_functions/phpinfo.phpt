--TEST--
phpinfo()
--FILE--
<?php
var_dump(phpinfo());

echo "--\n";
var_dump(phpinfo(array()));

echo "--\n";
var_dump(phpinfo(0));

echo "--\n";
var_dump(phpinfo(INFO_LICENSE));

?>
--EXPECTF--
phpinfo()
PHP Version => %s

System => %s
Build Date => %s
Configure Command => %s
Server API => Command Line Interface
Virtual Directory Support => %s
Configuration File (php.ini) Path => %s
Loaded Configuration File => %a
PHP API => %d
PHP Extension => %d
Zend Extension => %d
Debug Build => %s
Thread Safety => %s
Zend Memory Manager => %s
IPv6 Support => %s
Registered PHP Streams => %s
Registered Stream Socket Transports => %s
Registered Stream Filters => %s

%a
 _______________________________________________________________________


Configuration

PHP Core

%a

Additional Modules

%a

Environment

%a

PHP Variables

%a

PHP License
%a
bool(true)
--

Warning: phpinfo() expects parameter 1 to be long, array given in %sphpinfo.php on line 5
NULL
--
phpinfo()
bool(true)
--
phpinfo()

PHP License
%a
bool(true)
