--TEST--
SimpleXML: Simple document
--EXTENSIONS--
simplexml
--FILE--
<?php

var_dump(simplexml_load_file(__DIR__.'/sxe私はガラスを食べられます.xml'));

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
