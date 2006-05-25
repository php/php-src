--TEST--
Bug #37569 (WDDX incorrectly encodes high-ascii characters)
--FILE--
<?php
echo wddx_serialize_value(chr(1))."\n";
echo wddx_serialize_value(chr(128))."\n";
?>
--EXPECT--
<wddxPacket version='1.0'><header/><data><string><char code='01'/></string></data></wddxPacket>
<wddxPacket version='1.0'><header/><data><string><char code='80'/></string></data></wddxPacket>
