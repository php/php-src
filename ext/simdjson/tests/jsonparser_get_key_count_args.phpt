--TEST--
JsonParser::getKeyCount() args test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$reflection = new ReflectionMethod("JsonParser::getKeyCount");
var_dump((string) $reflection);

?>
--EXPECT--
string(253) "Method [ <internal:simdjson> static public method getKeyCount ] {

  - Parameters [3] {
    Parameter #0 [ <required> string $json ]
    Parameter #1 [ <required> string $key ]
    Parameter #2 [ <optional> int $depth = 512 ]
  }
  - Return [ mixed ]
}
"
