--TEST--
Bug #73173: huge memleak when wddx_unserialize
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php

$xml=<<<XML
<?xml version='1.0'?>
<!DOCTYPE wddxPacket SYSTEM 'wddx_0100.dtd'>
<wddxPacket>
<var name="
XML;

$xml .= str_repeat('F',0x80000);

$xml .= <<<XML
">
</wddxPacket>
XML;
var_dump(wddx_deserialize($xml));

?>
--EXPECT--
NULL
