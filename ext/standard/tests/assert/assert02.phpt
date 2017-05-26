--TEST--
catch assert() errors
--INI--
assert.active=1
assert.warning=1
assert.callback=
assert.bail=0
assert.quiet_eval=0
--FILE--
<?php

assert(1);
assert('1');
assert('$a');

try {
	assert('aa=sd+as+safsafasfasafsaf');
} catch (Throwable $e) {
	echo $e->getMessage(), "\n";
}

assert('0');

assert_options(ASSERT_BAIL, 1);

try {
	assert('aa=sd+as+safsafasfasafsaf');
} catch (Throwable $e) {
	echo $e->getMessage(), "\n";
}

echo "done\n";

?>
--EXPECTF--
Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d

Notice: Undefined variable: a in %sassert02.php(%d) : assert code on line 1

Warning: assert(): Assertion "$a" failed in %sassert02.php on line %d

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d
Failure evaluating code: 
aa=sd+as+safsafasfasafsaf

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d

Warning: assert(): Assertion "0" failed in %sassert02.php on line %d

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d

Fatal error: Uncaught ParseError: syntax error, unexpected '=', expecting ';' in %s(%d) : assert code:1
Stack trace:
#0 %s(%d): assert('aa=sd+as+safsaf...')
#1 {main}

Next Error: Failure evaluating code: 
aa=sd+as+safsafasfasafsaf in %s:%d
Stack trace:
#0 %s(%d): assert('aa=sd+as+safsaf...')
#1 {main}
  thrown in %s on line %d
