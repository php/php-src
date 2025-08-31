--TEST--
Bug #72588 (Using global var doesn't work while accessing SimpleXML element)
--EXTENSIONS--
simplexml
--FILE--
<?php

$tpnb = 5;
$dummy = &$tpnb;

$xmlStruct = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<templates>
    <object type="obj_1" label="Label for object 1"></object>
    <object type="obj_2" label="Label for object 2"></object>
    <object type="obj_3" label="Label for object 3"></object>
    <object type="obj_4" label="Label for object 4"></object>
    <object type="obj_5" label="Label for object 5"></object>
    <object type="obj_6" label="Label for object 6"></object>
    <object type="obj_7" label="Label for object 7"></object>
    <object type="obj_8" label="Label for object 8"></object>
    <object type="obj_9" label="Label for object 9"></object>
    <object type="obj_10" label="Label for object 10"></object>
</templates>
EOF;
$tplxml = simplexml_load_string($xmlStruct);

var_dump($tplxml->object[$tpnb]);
?>
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  ["@attributes"]=>
  array(2) {
    ["type"]=>
    string(5) "obj_6"
    ["label"]=>
    string(18) "Label for object 6"
  }
}
