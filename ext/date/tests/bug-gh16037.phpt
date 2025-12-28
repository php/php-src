--TEST--
Test for bug GH-16037: Assertion failure in ext/date/php_date.c
--FILE--
<?php
$di = (new DateInterval('P1Y'))->__unserialize([[]]);
echo gettype($di);
?>
--EXPECT--
NULL
