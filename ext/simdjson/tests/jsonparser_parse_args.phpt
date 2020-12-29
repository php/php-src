--TEST--
JsonParser::parse() args test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$reflection = new ReflectionMethod("JsonParser::parse");
var_dump((string) $reflection);

?>
--EXPECT--
string(308) "Method [ <internal:simdjson> static public method parse ] {

  - Parameters [4] {
    Parameter #0 [ <required> string $json ]
    Parameter #1 [ <optional> bool $associative = false ]
    Parameter #2 [ <optional> int $depth = 512 ]
    Parameter #3 [ <optional> int $flags = 0 ]
  }
  - Return [ mixed ]
}
"
