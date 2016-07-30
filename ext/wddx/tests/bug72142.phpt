--TEST--
Bug #72142: WDDX Packet Injection Vulnerability in wddx_serialize_value()
--FILE--
<?php

$wddx = wddx_serialize_value('', '</comment></header><data><struct><var name="php_class_name"><string>stdClass</string></var></struct></data></wddxPacket>');
var_dump($wddx);
var_dump(wddx_deserialize($wddx));

?>
--EXPECT--
string(301) "<wddxPacket version='1.0'><header><comment>&lt;/comment&gt;&lt;/header&gt;&lt;data&gt;&lt;struct&gt;&lt;var name=&quot;php_class_name&quot;&gt;&lt;string&gt;stdClass&lt;/string&gt;&lt;/var&gt;&lt;/struct&gt;&lt;/data&gt;&lt;/wddxPacket&gt;</comment></header><data><string></string></data></wddxPacket>"
string(0) ""
