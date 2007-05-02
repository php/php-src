--TEST--
catch assert() errors
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

assert('aa=sd+as+safsafasfaçsafçsafç');

assert('0');

assert_options(ASSERT_BAIL, 1);
assert('aa=sd+as+safsafasfaçsafçsafç');

echo "done\n";

?>
--EXPECTF--
Notice: Undefined variable: a in %sassert02.php(12) : assert code on line 1

Warning: assert(): Assertion "$a" failed in %sassert02.php on line %d

Parse error: %s error%sassert02.php(%d) : assert code on line 1
in handler()
string(%d) "assert(): Failure evaluating code: 
aa=sd+as+safsafasfaçsafçsafç"

Warning: assert(): Assertion "0" failed in %sassert02.php on line %d

Parse error: %s error%sassert02.php(%d) : assert code on line 1
in handler()
string(%d) "assert(): Failure evaluating code: 
aa=sd+as+safsafasfaçsafçsafç"
