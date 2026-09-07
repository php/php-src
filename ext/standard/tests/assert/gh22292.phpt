--TEST--
GH-22292: AST pretty printing does not correctly handle invalid variable names
--FILE--
<?php

class Foo {
	public function __get($name) { return $name; }
}

try {
	${'---'} = 'abc';
	var_dump(${'---'});
	assert(!${'---'});
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	$f = new Foo();
	var_dump($f->{'---'});
	assert(!$f->{'---'});
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
string(3) "abc"
assert(!${'---'})
string(3) "---"
assert(!$f->{'---'})
