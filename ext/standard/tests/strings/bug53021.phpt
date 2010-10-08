--TEST--
Bug #53021 (Failure to convert numeric entities with ENT_NOQUOTES and ISO-8859-1)
--FILE--
<?php
var_dump(unpack("H*",html_entity_decode("&#233;", ENT_QUOTES, "ISO-8859-1")));
--EXPECT--
array(1) {
  [1]=>
  string(2) "e9"
}
