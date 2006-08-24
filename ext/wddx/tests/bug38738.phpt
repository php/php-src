--TEST--
Bug #38378 (wddx_serialize_vars() generates no wellformed xml)
--FILE--
<?php
$hash = array();
$hash["int"] = 1;
$hash["string"] = "test";

var_dump(wddx_serialize_vars('hash'));
?>
--EXPECT--
string(204) "<wddxPacket version='1.0'><header/><data><struct><var name='hash'><struct><var name='int'><number>1</number></var><var name='string'><string>test</string></var></struct></var></struct></data></wddxPacket>"
