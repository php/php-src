--TEST--
catch assert() errors
--INI--
assert.active=1
assert.warning=1
assert.callback=
assert.bbail=0
assert.quiet_eval=0
--FILE--
<?php
function handler($errno, $errstr) {
	echo "in handler()\n";
	assert(E_RECOVERABLE_ERROR === $errno);
	var_dump($errstr);
}

set_error_handler('handler', E_RECOVERABLE_ERROR);

assert(1);
assert('1');
assert('$a');

assert('aa=sd+as+safsafasfasafsaf');

assert('0');

assert_options(ASSERT_BAIL, 1);
assert('aa=sd+as+safsafasfasafsaf');

echo "done\n";

?>
--EXPECTF--
Notice: Undefined variable: a in %sassert02.php(12) : assert code on line 1

Warning: assert(): Assertion "$a" failed in %sassert02.php on line 12

Parse error: %s error%sin %sassert02.php(14) : assert code on line 1
in handler()
%string|unicode%(%d) "assert(): Failure evaluating code: 
aa=sd+as+safsafasfasafsaf"

Warning: assert(): Assertion "0" failed in %sassert02.php on line 16

Parse error: %s error%sin %sassert02.php(19) : assert code on line 1
in handler()
%string|unicode%(%d) "assert(): Failure evaluating code: 
aa=sd+as+safsafasfasafsaf"
