--TEST--
Bug #72799: wddx_deserialize null dereference in php_wddx_pop_element
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
<wddxPacket version="1.0">
    <var name="XXXX">
        <boolean value="1">
            <dateTime>1998-06-12T04:32:12+00</dateTime>
        </boolean>
    </var>
</wddxPacket>
XML;

$array = wddx_deserialize($xml);
var_dump($array);
?>
--EXPECT--
NULL