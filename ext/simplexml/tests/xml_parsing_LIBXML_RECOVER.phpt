--TEST--
XML parsing with LIBXML_RECOVER
--EXTENSIONS--
simplexml
--FILE--
<?php

var_dump(simplexml_load_string('<root><child/>', options: LIBXML_RECOVER));

?>
--EXPECTF--
Warning: simplexml_load_string(): %s

Warning: simplexml_load_string(): <root><child/> in %s on line %d

Warning: simplexml_load_string():               ^ in %s on line %d
object(SimpleXMLElement)#1 (1) {
  ["child"]=>
  object(SimpleXMLElement)#2 (0) {
  }
}
