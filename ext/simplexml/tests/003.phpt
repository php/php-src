--TEST--
SimpleXML: Entities
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml =<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd" [
<!ENTITY included-entity "This is text included from an entity">
]>
<sxe id="elem1">
 Plain text.
 <elem1 attr1='first'>
  <!-- comment -->
  <elem2>
   <elem3>
    &included-entity;
    <elem4>
     <?test processing instruction ?>
    </elem4>
   </elem3>
  </elem2>
 </elem1>
</sxe>
EOF;

var_dump(simplexml_load_string($xml));

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
      object(SimpleXMLElement)#%d (2) {
        ["included-entity"]=>
        object(SimpleXMLElement)#%d (1) {
          ["included-entity"]=>
          string(36) "This is text included from an entity"
        }
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
