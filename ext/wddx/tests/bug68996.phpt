--TEST--
Bug #68996 (Invalid free of CG(interned_empty_string))
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") !== "0")
    print "skip Need Zend MM disabled";
?>
--FILE--
<?php
echo wddx_serialize_value("\xfc\x63") . "\n";
echo wddx_serialize_value([ "\xfc\x63" => "foo" ]) . "\n";
?>
--EXPECT--
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
<wddxPacket version='1.0'><header/><data><struct><var name=''><string>foo</string></var></struct></data></wddxPacket>
