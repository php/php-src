--TEST--
Bug #73065: Out-Of-Bounds Read in php_wddx_push_element of wddx.c
--SKIPIF--
<?php
if (!extension_loaded('wddx')) {
    die('skip. wddx not available');
}
?>
--FILE--
<?php

$xml1 = <<<XML
<?xml version='1.0' ?>
    <!DOCTYPE et SYSTEM 'w'>
    <wddxPacket ven='1.0'>
        <array>
            <var Name="name">
                <boolean value="keliu"></boolean>
            </var>
            <var name="1111">
                <var name="2222">
                    <var name="3333"></var>
                </var>
            </var>
        </array>
    </wddxPacket>
XML;

$xml2 = <<<XML
<?xml version='1.0' ?>
    <!DOCTYPE et SYSTEM 'w'>
    <wddxPacket ven='1.0'>
        <array>
            <char Name="code">
                <boolean value="keliu"></boolean>
            </char>
        </array>
    </wddxPacket>
XML;

$xml3 = <<<XML
<?xml version='1.0' ?>
    <!DOCTYPE et SYSTEM 'w'>
    <wddxPacket ven='1.0'>
        <array>
            <boolean Name="value">
                <boolean value="keliu"></boolean>
            </boolean>
        </array>
    </wddxPacket>
XML;

$xml4 = <<<XML
<?xml version='1.0' ?>
    <!DOCTYPE et SYSTEM 'w'>
    <wddxPacket ven='1.0'>
        <array>
            <recordset Name="fieldNames">
                <boolean value="keliu"></boolean>
            </recordset>
        </array>
    </wddxPacket>
XML;

$xml5 = <<<XML
<?xml version='1.0' ?>
    <!DOCTYPE et SYSTEM 'w'>
    <wddxPacket ven='1.0'>
        <array>
            <field Name="name">
                <boolean value="keliu"></boolean>
            </field>
        </array>
    </wddxPacket>
XML;

for($i=1;$i<=5;$i++) {
	$xmlvar = "xml$i";
    $array = wddx_deserialize($$xmlvar);
    var_dump($array);
}
?>
DONE
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  array(0) {
  }
}
array(0) {
}
DONE
