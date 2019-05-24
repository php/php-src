--TEST--
Bug #76451: Aliases during inheritance type checks affected by opcache (variation)
--FILE--
<?php
class Foo {}
class_alias('Foo', 'Bar');

require __DIR__ . '/bug76451_2.inc';
?>
===DONE===
--EXPECT--
===DONE===
