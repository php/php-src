--TEST--
assert()
--INI--
error_reporting = 2039
assert.active = 0
assert.warning = 1
assert.callback = 
assert.bail = 0
assert.quiet_eval = 0
--FILE--
<?php
function a($file,$line,$myev)
{ 
	echo "assertion failed $line,\"$myev\"\n";
}

class a
{
	function assert($file,$line,$myev)
	{
		echo "class assertion failed $line,\"$myev\"\n";
	}
}

assert_options(ASSERT_ACTIVE,1);
assert_options(ASSERT_QUIET_EVAL,1);
assert_options(ASSERT_WARNING,0);

$a = 0;

assert_options(ASSERT_CALLBACK,"a");
assert('$a != 0');

assert_options(ASSERT_CALLBACK,array("a","assert"));
assert('$a != 0');

$obj = new a();
assert_options(ASSERT_CALLBACK,array(&$obj,"assert"));
assert('$a != 0');
?>
--EXPECT--
assertion failed 22,"$a != 0"
class assertion failed 25,"$a != 0"
class assertion failed 29,"$a != 0"
