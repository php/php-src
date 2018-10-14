--TEST--
Bug #72860: wddx_deserialize use-after-free
--SKIPIF--
<?php
if (!extension_loaded('wddx')) {
    die('skip. wddx not available');
}
?>
--FILE--
<?php

$xml=<<<XML
<?xml version='1.0'?>
<!DOCTYPE wddxPacket SYSTEM 'wddx_0100.dtd'>
<wddxPacket version='1.0'>
       <recordset fieldNames='F'>
               <field name='F'>
       </recordset>
</wddxPacket>
XML;

var_dump(wddx_deserialize($xml));
?>
DONE
--EXPECT--
NULL
DONE
