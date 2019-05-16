--TEST--
Bug #31875 get_defined_functions() should not list disabled functions
--CREDITS--
Willian Gustavo Veiga <contact@willianveiga.com>
--INI--
disable_functions=dl
--FILE--
<?php
$disabled_function = 'dl';

$functions = get_defined_functions();
var_dump(in_array($disabled_function, $functions['internal']));

$functions = get_defined_functions(false);
var_dump(in_array($disabled_function, $functions['internal']));

$functions = get_defined_functions(true);
var_dump(in_array($disabled_function, $functions['internal']));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
