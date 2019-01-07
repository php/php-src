--TEST--
Bug #73631 (Memory leak due to invalid wddx stack processing)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
---XFAIL--
Still has memory leaks in debug build.
--FILE--
<?php
$xml = <<<EOF
<?xml version="1.0" ?>
<wddxPacket version="1.0">
<number>1234</number>
<binary><boolean/></binary>
</wddxPacket>
EOF;
$wddx = wddx_deserialize($xml);
var_dump($wddx);
?>
--EXPECTF--
int(1234)
