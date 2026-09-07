--TEST--
'break' error (operator with non-integer operand)
--FILE--
<?php
function foo () {
    break $x;
}
?>
--EXPECTF--
Fatal error: 'break' operator with non-integer operand is no longer supported in %sbreak_error_002.php on line 3
