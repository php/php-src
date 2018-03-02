--TEST--
Bug #71587 (Use-After-Free / Double-Free in WDDX Deserialize)
--SKIPIF--
<?php
if (!extension_loaded("wddx")) print "skip";
?>
--FILE--
<?php

$xml = <<<EOF
<?xml version='1.0' ?>
<!DOCTYPE wddxPacket SYSTEM 'wddx_0100.dtd'>
<wddxPacket version='1.0'>
    <array>
         <var name='ML'></var>
            <string>manhluat</string>
         <var name='ML2'></var>
         	<boolean value='a'/>
         <boolean value='true'/>
    </array>
</wddxPacket>
EOF;

$wddx = wddx_deserialize($xml);
var_dump($wddx);
// Print mem leak
foreach($wddx as $k=>$v)
	printf("Key: %s\nValue: %s\n",bin2hex($k),bin2hex($v));

?>
DONE
--EXPECT--
array(2) {
  [0]=>
  string(8) "manhluat"
  [1]=>
  bool(true)
}
Key: 30
Value: 6d616e686c756174
Key: 31
Value: 31
DONE
