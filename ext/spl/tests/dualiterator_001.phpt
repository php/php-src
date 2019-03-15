--TEST--
SPL: DualIterator
--FILE--
<?php

function spl_examples_autoload($classname)
{
	include(__DIR__ . '/' . strtolower($classname) . '.inc');
}

spl_autoload_register('spl_examples_autoload');

function test($a, $b, $identical = false)
{
	var_dump(DualIterator::compareIterators(
					new RecursiveArrayIterator($a),
					new RecursiveArrayIterator($b),
					$identical));
}

test(array(1,2,3), array(1,2,3));
test(array(1,2,3), array(1,2));
test(array(1,array(21,22),3), array(1,array(21,22),3));
test(array(1,array(21,22),3), array(1,array(21,22,23),3));
test(array(1,array(21,22),3), array(1,array(21,22,3)));
test(array(1,array(21,22),3), array(1,array(21),array(22),3));
test(array(1,2,3), array(1,"2",3), false);
test(array(1,2,3), array(1,"2",3), true);
test(array(1,array(21,22),3), array(1,array(21,"22"),3), false);
test(array(1,array(21,22),3), array(1,array(21,"22"),3), true);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
===DONE===
