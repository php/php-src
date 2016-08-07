--TEST--
Bug #72750: wddx_deserialize null dereference
--SKIPIF--
<?php
if (!extension_loaded('wddx')) {
    die('skip. wddx not available');
}
?>
--FILE--
<?php

$xml = <<< XML
<?xml version='1.0'?>
<!DOCTYPE wddxPacket SYSTEM 'wddx_0100.dtd'>
<wddxPacket version='1.0'>
<header/>
        <data>
                <struct>
                     <var name='aBinary'>
                         <binary length='11'>\\tYmluYXJRhdGE=</binary>
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
  ["aBinary"]=>
  string(0) ""
}
