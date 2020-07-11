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
} catch (InvalidArgumentException $e) {
    var_dump($e->getMessage());
    var_dump($e->getCode());
}

?>
--EXPECT--
int(0)
string(14) "Illegal mode 7"
int(0)
