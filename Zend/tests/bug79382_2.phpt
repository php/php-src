--TEST--
Bug #79382: Cannot redeclare disabled function
--INI--
disable_functions=substr
--FILE--
<?php

function substr() {
	return 'foo';
}

var_dump(substr("bar"));

?>
--EXPECT--
string(3) "foo"
