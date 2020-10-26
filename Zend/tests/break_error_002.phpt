--TEST--
'break' error (operator with non-integer operand)
--FILE--
<?php
function foo () {
    break $x;
}
?>
--EXPECTF--
Fatal error: The break statement with non-integer argument is no longer supported in %s on line %d
