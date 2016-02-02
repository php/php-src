--TEST--
Bug #71335 (Type Confusion in WDDX Packet Deserialization)
--SKIPIF--
<?php
if (!extension_loaded("wddx")) print "skip";
?>
--FILE--
<?php
$x = "<?xml version='1.0'?>
<wddxPacket version='1.0'>
<header/>
	<data>
		<struct>
			<var name='php_class_name'>
				<string>stdClass</string>
			</var>
			<var name='php_class_name'>
				<string>stdClass</string>
			</var>
		</struct>
	</data>
</wddxPacket>";

$d = wddx_deserialize($x);
var_dump($d);
?>
DONE
--EXPECTF--
object(stdClass)#%d (1) {
  ["php_class_name"]=>
  string(8) "stdClass"
}
DONE
