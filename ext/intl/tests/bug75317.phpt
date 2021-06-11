--TEST--
Bug #75317 (UConverter::setDestinationEncoding changes source instead of destinatination)
--EXTENSIONS--
intl
--FILE--
<?php
$utf8 = UConverter::getAliases('utf-8')[0];
$utf16 = UConverter::getAliases('utf-16')[0];
$utf32 = UConverter::getAliases('utf-32')[0];
$latin1 = UConverter::getAliases('latin1')[0];

function printResult($actual, $expected) {
    var_dump($actual === $expected ? true : "expected: $expected, actual: $actual");
}

// test default values
$c = new UConverter();
printResult($c->getDestinationEncoding(), $utf8);
printResult($c->getSourceEncoding(), $utf8);

// test constructor args
$c = new UConverter('utf-16', 'latin1');
printResult($c->getDestinationEncoding(), $utf16);
printResult($c->getSourceEncoding(), $latin1);

// test setters
var_dump($c->setDestinationEncoding('utf-8'));
var_dump($c->setSourceEncoding('utf-32'));
printResult($c->getDestinationEncoding(), $utf8);
printResult($c->getSourceEncoding(), $utf32);

// test invalid inputs dont change values
var_dump($c->setDestinationEncoding('foobar') === false);
var_dump($c->setSourceEncoding('foobar') === false);
printResult($c->getDestinationEncoding(), $utf8);
printResult($c->getSourceEncoding(), $utf32);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
