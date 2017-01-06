--TEST--
Bug #72340: Double Free Courruption in wddx_deserialize
--SKIPIF--
<?php
if (!extension_loaded("wddx")) print "skip";
?>
--FILE--
<?php
$xml = <<<EOF
<?xml version='1.0' ?>
<!DOCTYPE wddxPacket SYSTEM 'wddx_0100.dtd'>
<wddxPacket version='1.0'>
	<array><var name="XXXXXXXX"><boolean value="none">TEST</boolean></var>
		<var name="YYYYYYYY"><var name="ZZZZZZZZ"><var name="EZEZEZEZ">
		</var></var></var>
	</array>
</wddxPacket>
EOF;
$array = wddx_deserialize($xml);
var_dump($array);
?>
--EXPECT--
array(0) {
}
