--TEST--
Bug #68162: isset($$varname) always true
--FILE--
<?php

$name = 'var';
var_dump(isset($$name));
$var = 42;
var_dump(isset($$name));

?>
--EXPECT--
bool(false)
bool(true)
