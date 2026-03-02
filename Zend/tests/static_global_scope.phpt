--TEST--
Using 'static' and 'global' in global scope
--FILE--
<?php

static $var = -1;
var_dump($var);

global $var, $var, $var;
var_dump($var);

var_dump($GLOBALS['var']);

?>
--EXPECT--
int(-1)
int(-1)
int(-1)
