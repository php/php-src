--TEST--
SPL: RegexIterator::setMode() error tests
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php

$array = array('foo', 'bar', 'baz');
$regexIterator = new RegexIterator(new ArrayIterator($array), "/f/");

var_dump($regexIterator->getMode());

try {
	$regexIterator->setMode(7);
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
int(0)
Illegal mode 7
