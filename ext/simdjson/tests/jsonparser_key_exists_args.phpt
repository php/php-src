--TEST--
JsonParser::keyExists() args test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$reflection = new ReflectionMethod("JsonParser::keyExists");
var_dump((string) $reflection);

?>
--EXPECT--
string(251) "Method [ <internal:simdjson> static public method keyExists ] {

  - Parameters [3] {
    Parameter #0 [ <required> string $json ]
    Parameter #1 [ <required> string $key ]
    Parameter #2 [ <optional> int $depth = 512 ]
  }
  - Return [ ?bool ]
}
"
