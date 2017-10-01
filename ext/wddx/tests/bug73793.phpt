--TEST--
Bug #73793 (WDDX uses wrong decimal seperator)
--SKIPIF--
<?php
if (!extension_loaded('wddx')) print 'skip wddx extension not available';
if (setlocale(LC_NUMERIC, ['de_DE', 'de_DE.UTF-8', 'de-DE']) === false) {
    print 'skip German locale not available';
}
?>
--FILE--
<?php
setlocale(LC_NUMERIC , ['de_DE', 'de_DE.UTF-8', 'de-DE']);
var_dump(wddx_serialize_value(['foo' => 5.1]));
?>
===DONE===
--EXPECT--
string(120) "<wddxPacket version='1.0'><header/><data><struct><var name='foo'><number>5.1</number></var></struct></data></wddxPacket>"
===DONE===
