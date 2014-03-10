--TEST--
Unserialization of partial strings
--FILE--
<?php
$data = [123,4.56,true];
$ser = serialize($data);
$serlen = strlen($ser);

$unser = unserialize($ser, $consumed);
echo "Consume full string: ";
var_dump($serlen == $consumed);
echo "Return original data: ";
var_dump($unser === $data);

$ser .= "junk\x01data";
$unser = unserialize($ser, $consumed);
echo "Consume full string(junk): ";
var_dump($serlen == $consumed);
echo "Return original data(junk): ";
var_dump($unser === $data);

--EXPECT--
Consume full string: bool(true)
Return original data: bool(true)
Consume full string(junk): bool(true)
Return original data(junk): bool(true)

