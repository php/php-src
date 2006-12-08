--TEST--
assert() - set callback using ini_set()
--FILE--
<?php
function a($file,$line,$myev)
{ 
	        echo "assertion failed - a - $line,\"$myev\"\n";
}
 
function b($file,$line,$myev)
{ 
	        echo "assertion failed - b - $line,\"$myev\"\n";
}
 
assert_options(ASSERT_ACTIVE,1);
assert_options(ASSERT_QUIET_EVAL,1);
assert_options(ASSERT_WARNING,0);
 
$a = 0;
 
assert_options(ASSERT_CALLBACK,"a");
assert('$a != 0');
 
 /* Modify call back using ini_set() */
ini_set("assert.callback", "b");
assert('$a != 0');

?>
==DONE==
--EXPECTF--
assertion failed - a - %d,"$a != 0"
assertion failed - b - %d,"$a != 0"
==DONE==
