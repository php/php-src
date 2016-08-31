--TEST--
Bug #72749: wddx_deserialize allows illegal memory access
--SKIPIF--
<?php
if (!extension_loaded('wddx')) {
    die('skip. wddx not available');
}
?>
--FILE--
<?php
$xml = <<<XML
<?xml version='1.0'?>
<!DOCTYPE wddxPacket SYSTEM 'wddx_0100.dtd'>
<wddxPacket version='1.0'>
<header/>
        <data>
                <struct>
                     <var name='aDateTime3'>
                         <dateTime>2\r2004-09-10T05:52:49+00</dateTime>
                     </var>
                 </struct>
        </data>
</wddxPacket>
XML;

$array = wddx_deserialize($xml);
var_dump($array);
?>
--EXPECT--
array(1) {
  ["aDateTime3"]=>
  string(24) "2
2004-09-10T05:52:49+00"
}
