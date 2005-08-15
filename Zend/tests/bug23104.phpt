--TEST--
Bug #23104 (Hash position not reset for constant arrays)
--FILE--
<?php
function foo($bar = array("a", "b", "c"))
{
	var_dump(current($bar));
}
foo();
?>
--EXPECT--
string(1) "a"
--UEXPECT--
unicode(1) "a"
