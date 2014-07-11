--TEST--
Bug #64515 (Memoryleak when using the same variablename 2times in function declaration)
--FILE--
<?php
function foo($unused = null, $unused = null, $arg = array()) {
	    return 1;
}
foo();
echo "okey";
?>
--EXPECT--
okey
