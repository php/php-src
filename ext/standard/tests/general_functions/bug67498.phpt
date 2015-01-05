--TEST--
phpinfo() Type Confusion Information Leak Vulnerability
--FILE--
<?php
$PHP_SELF = 1;
phpinfo(INFO_VARIABLES);

?>
==DONE==
--EXPECTF--
phpinfo()

PHP Variables
%A
==DONE==
