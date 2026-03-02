--TEST--
SCCP 040: Memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function f() {
    $y[] = $arr[] = array($y);
    $arr();
}
f();
?>
--EXPECTF--
Warning: Undefined variable $y in %ssccp_040.php on line 3

Fatal error: Uncaught Error: Array callback must have exactly two elements in %ssccp_040.php:4
Stack trace:
#0 %ssccp_040.php(6): f()
#1 {main}
  thrown in %ssccp_040.php on line 4
