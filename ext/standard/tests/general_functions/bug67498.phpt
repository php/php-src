--TEST--
phpinfo() Type Confusion Information Leak Vulnerability
--FILE--
<?php
$PHP_SELF = 1;
phpinfo(INFO_VARIABLES);

?>
--EXPECTF--
phpinfo()

PHP Variables
%A
