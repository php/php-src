--TEST--
JsonParser::isValid() args test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$reflection = new ReflectionMethod("JsonParser::isValid");
var_dump((string) $reflection);

?>
--EXPECT--
string(155) "Method [ <internal:simdjson> static public method isValid ] {

  - Parameters [1] {
    Parameter #0 [ <required> string $json ]
  }
  - Return [ bool ]
}
"
