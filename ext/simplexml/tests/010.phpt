--TEST--
SimpleXML: Simple Inheritance
--EXTENSIONS--
simplexml
--FILE--
<?php

class simplexml_inherited extends SimpleXMLElement
{
}

$xml =<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd">
<sxe id="elem1">
 <elem1 attr1='first'>
  <!-- comment -->
  <elem2>
   <elem3>
    <elem4>
     <?test processing instruction ?>
    </elem4>
   </elem3>
  </elem2>
 </elem1>
</sxe>
EOF;

var_dump(simplexml_load_string($xml, 'simplexml_inherited'));

?>
--EXPECTF--
object(simplexml_inherited)#%d (2) {
  ["@attributes"]=>
  array(1) {
    ["id"]=>
    string(5) "elem1"
  }
  ["elem1"]=>
  object(simplexml_inherited)#%d (3) {
    ["@attributes"]=>
    array(1) {
      ["attr1"]=>
      string(5) "first"
    }
    ["comment"]=>
    object(simplexml_inherited)#%d (0) {
    }
    ["elem2"]=>
    object(simplexml_inherited)#%d (1) {
      ["elem3"]=>
      object(simplexml_inherited)#%d (1) {
        ["elem4"]=>
        object(simplexml_inherited)#%d (1) {
          ["test"]=>
          object(simplexml_inherited)#%d (0) {
          }
        }
      }
    }
  }
}
