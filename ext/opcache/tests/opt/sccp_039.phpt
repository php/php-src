--TEST--
SCCP 039: Incorrect constant propagation for VERIFY_RETURN_TYPE
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo(): null {
    return Y;
}
?>
DONE
--EXPECT--
DONE
