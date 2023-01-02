--TEST--
Bug #73668: "SIGFPE Arithmetic exception" in opcache when divide by minus 1
--EXTENSIONS--
opcache
--FILE--
<?php
$a/-1;
?>
--EXPECTF--
Warning: Undefined variable $a (this will become an error in PHP 9.0) in %s on line %d
