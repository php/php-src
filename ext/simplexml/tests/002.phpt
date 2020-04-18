--TEST--
SimpleXML: clone
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

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

$sxe = simplexml_load_string($xml);

$copy = clone $sxe;

var_dump($copy);

?>
--EXPECTF--
object(SimpleXMLElement)#%d (2) {
  ["@attributes"]=>
  array(1) {
    ["id"]=>
    string(5) "elem1"
  }
  ["elem1"]=>
  object(SimpleXMLElement)#%d (3) {
    ["@attributes"]=>
    array(1) {
      ["attr1"]=>
      string(5) "first"
    }
    ["comment"]=>
    object(SimpleXMLElement)#%d (0) {
    }
    ["elem2"]=>
    object(SimpleXMLElement)#%d (1) {
      ["elem3"]=>
      object(SimpleXMLElement)#%d (1) {
        ["elem4"]=>
        object(SimpleXMLElement)#%d (1) {
          ["test"]=>
          object(SimpleXMLElement)#%d (0) {
          }
        }
      }
    }
  }
}
