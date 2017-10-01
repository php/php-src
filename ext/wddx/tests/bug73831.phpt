--TEST--
Bug #73831 (NULL Pointer Dereference while unserialize php object)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php
$xml = <<<EOF
<?xml version="1.0" ?>
<wddxPacket version="1.0">
	<struct>
		<var name="php_class_name">
			<string>Throwable</string>
                </var>
        </struct>
</wddxPacket>
EOF;
try {
	$wddx = wddx_deserialize($xml);
} catch(Error $e) { echo $e->getMessage(); }
?>
--EXPECTF--
Warning: wddx_deserialize(): Class throwable can not be instantiated in %sbug73831.php on line %d
Cannot instantiate interface Throwable
