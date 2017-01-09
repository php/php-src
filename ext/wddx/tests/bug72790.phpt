--TEST--
Bug #72790: wddx_deserialize null dereference with invalid xml
--SKIPIF--
<?php
if (!extension_loaded('wddx')) {
    die('skip. wddx not available');
}
?>
--FILE--
<?php

$xml = <<< XML
<?xml version='1.0' ?>
<!DOCTYPE wddxPacket SYSTEM 'wddx_0100.dtd'>
<wddxPacket version='1.0'>
        |array>
                <var name="XXXX">
                        <boolean value="this">
                        </boolean>
                </var>
                <var name="YYYY">
                        <var name="UUUU">
                                <var name="EZEZ">
                                </var>
                        </var>
                </var>
        </array>
</wddxPacket>
XML;

$array = wddx_deserialize($xml);
var_dump($array);
?>
--EXPECT--
NULL