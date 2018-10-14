--TEST--
Bug #73331 (NULL Pointer Dereference in WDDX Packet Deserialization with PDORow)
--SKIPIF--
<?php if (!extension_loaded("wddx") || !extension_loaded("pdo")) print "skip"; ?>
--FILE--
<?php

$wddx = "<wddxPacket version='1.0'><header/><data><struct><var name='php_class_name'><string>PDORow</string></var></struct></data></wddxPacket>";
var_dump(wddx_deserialize($wddx));
?>
--EXPECTF--
Warning: wddx_deserialize(): Class pdorow can not be unserialized in %s73331.php on line %d
NULL
