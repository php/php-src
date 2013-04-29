--TEST--
SPL: RegexIterator::getRegex() basic tests
--CREDITS--
Joshua Thijssen <jthijssen@noxlogic.nl>
--FILE--
<?php

$array = array('cat', 'hat', 'sat');
$iterator = new ArrayIterator($array);

# Simple regex
$regexIterator = new RegexIterator($iterator, '/.at/');
var_dump($regexIterator->getRegex());

# Empty regular expression
$regexIterator = new RegexIterator($iterator, '//');
var_dump($regexIterator->getRegex());

# "Complex" email regular expression
$regexIterator = new RegexIterator($iterator, '|\b[A-Z0-9._%-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b|');
var_dump($regexIterator->getRegex());



?>
--EXPECT--
string(5) "/.at/"
string(2) "//"
string(43) "|\b[A-Z0-9._%-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b|"
