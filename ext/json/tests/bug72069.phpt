--TEST--
Bug #72069 (Behavior \JsonSerializable different from json_encode)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$result = json_encode(['end' => json_decode(null, true)]);
var_dump($result);

class A implements \JsonSerializable
{
	function jsonSerialize()
	{
		return ['end' => json_decode(null, true)];
	}
}
$a = new A();
$toJsonData = $a->jsonSerialize();
$result = json_encode($a);
var_dump($result);

$result = json_encode($toJsonData);
var_dump($result);
?>
--EXPECT--
string(12) "{"end":null}"
string(12) "{"end":null}"
string(12) "{"end":null}"
