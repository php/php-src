--TEST--
(un)serializing __PHP_Incomplete_Class instance edge case
--FILE--
<?php

$serialized = 'O:8:"Missing_":1:{s:33:"__PHP_Incomplete_Class_Name' . "\0" . 'other";i:123;}';
ob_start();
$o = unserialize($serialized);
var_dump($o);
$reserialized = serialize($o);
var_dump(unserialize($reserialized));
// Pretty print null bytes
echo str_replace("\0", "\\0", ob_get_clean());

// The serialization should have a property count of 1 and a property set with 1 element.
var_export($reserialized);
echo "\n";
?>
--EXPECT--
object(__PHP_Incomplete_Class)#1 (2) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(8) "Missing_"
  ["__PHP_Incomplete_Class_Name\0other"]=>
  int(123)
}
object(__PHP_Incomplete_Class)#2 (2) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(8) "Missing_"
  ["__PHP_Incomplete_Class_Name\0other"]=>
  int(123)
}
'O:8:"Missing_":1:{s:33:"__PHP_Incomplete_Class_Name' . "\0" . 'other";i:123;}'
