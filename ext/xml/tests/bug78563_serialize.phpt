--TEST--
Bug #78563: parsers should not be serializable
--EXTENSIONS--
xml
--FILE--
<?php

$parser = xml_parser_create();
serialize($parser);

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught Exception: Serialization of 'XMLParser' is not allowed in %s:%d
Stack trace:
#0 %s(%d): serialize(Object(XMLParser))
#1 {main}
  thrown in %s on line %d
