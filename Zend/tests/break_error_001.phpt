--TEST--
'break' error (non positive integers)
--FILE--
<?php
function foo () {
    break 0;
}
?>
--EXPECTF--
Fatal error: The break statement accepts only an integer argument greater than or equal to 0 in %s on line %d
