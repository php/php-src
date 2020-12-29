--TEST--
JsonParser::getKeyValue() args test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$reflection = new ReflectionMethod("JsonParser::getKeyValue");
var_dump((string) $reflection);

?>
--EXPECT--
string(358) "Method [ <internal:simdjson> static public method getKeyValue ] {

  - Parameters [5] {
    Parameter #0 [ <required> string $json ]
    Parameter #1 [ <required> string $key ]
    Parameter #2 [ <optional> bool $associative = false ]
    Parameter #3 [ <optional> int $depth = 512 ]
    Parameter #4 [ <optional> int $flags = 0 ]
  }
  - Return [ mixed ]
}
"
