--TEST--
Bug #73113 (Segfault with throwing JsonSerializable)
Also test that the custom exception is not wrapped by ext/json
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

class JsonSerializableObject implements \JsonSerializable
{
    public function jsonSerialize()
    {
        throw new \Exception('This error is expected');
    }
}

$obj = new JsonSerializableObject();
try {
	echo json_encode($obj);
} catch (\Exception $e) {
	echo $e->getMessage();
}
--EXPECTF--
This error is expected
