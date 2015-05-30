--TEST--
Bug #31875 get_defined_functions() should not list disabled functions
--CREDITS--
Willian Gustavo Veiga <contact@willianveiga.com>
--INI--
disable_functions=dl
--FILE--
<?php
var_dump(in_array('dl', get_defined_functions()));
?>
--EXPECTF--
bool(false)
