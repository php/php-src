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
    echo $e->getMessage() . \PHP_EOL;
    var_dump($e->getCode());
}

?>
--EXPECT--
int(0)
RegexIterator::setMode(): Argument #1 ($mode) must be RegexIterator::MATCH, RegexIterator::GET_MATCH, RegexIterator::ALL_MATCHES, RegexIterator::SPLIT, or RegexIterator::REPLACE
int(0)
