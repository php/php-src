--TEST--
ISSET_ISEMPTY_VAR 001: CONST operand of ISSET_ISEMPTY_VAR must be converted to STRING
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
class A {
    function __destruct() {
        $i=0;
        if (isset($GLOBALS[$i])) y;
    }
}
new A;
?>
DONE
--EXPECT--
DONE
