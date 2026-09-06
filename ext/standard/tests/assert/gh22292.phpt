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
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	$f = new Foo();
	var_dump($f->{'---'});
	assert(!$f->{'---'});
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
string(3) "abc"
AssertionError: assert(!${'---'})
string(3) "---"
AssertionError: assert(!$f->{'---'})
