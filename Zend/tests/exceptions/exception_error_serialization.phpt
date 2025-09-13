--TEST--
Check exception Error can round trip serialization
--FILE--
<?php

$e1 = new Error("1", 25, null);
$e2 = new Error("2", 42, $e1);

$s = serialize($e2);

$e = unserialize($s);

var_dump($e->getMessage() === $e2->getMessage());
var_dump($e->getCode() === $e2->getCode());
var_dump($e->getFile() === $e2->getFile());
var_dump($e->getLine() === $e2->getLine());
var_dump($e->getTrace() === $e2->getTrace());
var_dump($e->getPrevious()::class === $e2->getPrevious()::class);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
