--TEST--
UConverter Algorithmic converters
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$c = new UConverter('utf-8', 'latin1');
var_dump(UConverter::LATIN_1 === $c->getSourceType());
var_dump(UConverter::UTF8    === $c->getDestinationType());

$c = new UConverter('koi8-r', 'utf-32be');
var_dump(UConverter::UTF32_BigEndian === $c->getSourceType());
var_dump(UConverter::SBCS            === $c->getDestinationType());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
