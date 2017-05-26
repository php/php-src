--TEST--
Bug #72564: wddx deserialization of boolean
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php
	foreach([true, false, NULL] as $v) {
		$x =  wddx_serialize_value($v);
		var_dump($x, wddx_deserialize($x));
	}
?>
Done
--EXPECT--
string(84) "<wddxPacket version='1.0'><header/><data><boolean value='true'/></data></wddxPacket>"
bool(true)
string(85) "<wddxPacket version='1.0'><header/><data><boolean value='false'/></data></wddxPacket>"
bool(false)
string(68) "<wddxPacket version='1.0'><header/><data><null/></data></wddxPacket>"
NULL
Done
