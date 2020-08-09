--TEST--
Bug #73113 (Segfault with throwing JsonSerializable)
Also test that the custom exception is not wrapped by ext/json
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
?>
--EXPECT--
This error is expected
