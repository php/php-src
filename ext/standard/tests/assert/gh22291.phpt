--TEST--
GH-22291: AST pretty printing does not correctly handle braces in string interpolation
--FILE--
<?php

try {
	$foo = 'abc';
	var_dump("{{$foo}}");
	var_dump("{$foo}");
	assert(!"{{$foo}}");
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
string(5) "{abc}"
string(3) "abc"
assert(!"{{$foo}}")
