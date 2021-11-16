--TEST--
Bug #69491 (simplexml doesn't correctly parse empty nodes on same line as another node)
--EXTENSIONS--
simplexml
--FILE--
<?php
var_dump(simplexml_load_string('<a>
  <b><c/></b>
</a>'));?>
--EXPECT--
object(SimpleXMLElement)#1 (1) {
  ["b"]=>
  object(SimpleXMLElement)#2 (1) {
    ["c"]=>
    object(SimpleXMLElement)#3 (0) {
    }
  }
}
