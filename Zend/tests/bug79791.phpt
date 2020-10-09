--TEST--
Bug #79791: Assertion failure when unsetting variable during binary op
--FILE--
<?php
set_error_handler(function() {
    unset($GLOBALS['c']);
});
$c -= 1;
var_dump($c);
?>
--EXPECT--
int(-1)
