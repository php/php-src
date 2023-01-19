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
--EXPECT--
Deprecated: Increment on non-alphanumeric string is deprecated in Unknown on line 0
DONE
