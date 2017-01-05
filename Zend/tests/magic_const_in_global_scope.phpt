--TEST--
Test use of magic constants in the global scope
--FILE--
<?php

var_dump(
    __LINE__,
    __FILE__,
    __DIR__,
    __FUNCTION__,
    __METHOD__,
    __CLASS__,
    __TRAIT__,
    __NAMESPACE__
);

?>
--EXPECTF--
int(4)
string(%d) "%s"
string(%d) "%s"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
