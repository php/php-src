--TEST--
Bug #66025 (Indent wrong when json_encode() called from jsonSerialize function)
--SKIPIF--
<?php
if (!extension_loaded('json')) die('skip');
?>
--FILE--
<?php

class Foo implements JsonSerializable {
    public function jsonSerialize() {
	    return json_encode([1], JSON_PRETTY_PRINT);
	}
}

echo json_encode([new Foo]), "\n";
?>
--EXPECT--
["[\n    1\n]"]
