--TEST--
Bug #39018 [2] (Error control operator '@' fails to suppress "Uninitialized string offset")
--FILE--
<?php

error_reporting(E_ALL);

$foo = 'test';
$x = @$foo[6];

var_dump(@($foo[100] . $foo[130]));

?>
--EXPECT--
string(0) ""
