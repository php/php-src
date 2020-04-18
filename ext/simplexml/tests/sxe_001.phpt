--TEST--
SPL: SimpleXMLIterator
--SKIPIF--
<?php
if (!extension_loaded("simplexml")) print "skip SimpleXML not present";
if (!extension_loaded("libxml")) print "skip LibXML not present";
?>
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

var_dump(simplexml_load_string($xml, 'SimpleXMLIterator'));

?>
--EXPECTF--
object(SimpleXMLIterator)#%d (2) {
  ["@attributes"]=>
  array(1) {
    ["id"]=>
    string(5) "elem1"
  }
  ["elem1"]=>
  object(SimpleXMLIterator)#%d (3) {
    ["@attributes"]=>
    array(1) {
      ["attr1"]=>
      string(5) "first"
    }
    ["comment"]=>
    object(SimpleXMLIterator)#%d (0) {
    }
    ["elem2"]=>
    object(SimpleXMLIterator)#%d (1) {
      ["elem3"]=>
      object(SimpleXMLIterator)#%d (1) {
        ["elem4"]=>
        object(SimpleXMLIterator)#%d (1) {
          ["test"]=>
          object(SimpleXMLIterator)#%d (0) {
          }
        }
      }
    }
  }
}
