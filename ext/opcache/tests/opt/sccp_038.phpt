--TEST--
SCCP 038: Memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo() {
    $obj = new stdClass;
    $obj->$b = ~$b = $a = '##';
    $obj->$a++;
}
foo();
?>
DONE
--EXPECTF--
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %ssccp_038.php on line 5
DONE
