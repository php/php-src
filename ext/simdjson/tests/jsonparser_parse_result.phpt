--TEST--
JsonParser::parse() test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = file_get_contents(__DIR__ . DIRECTORY_SEPARATOR . "_files" . DIRECTORY_SEPARATOR . "result.json");
$value = JsonParser::parse($json, false);
var_dump($value);

$value = JsonParser::parse($json, true);
var_dump($value);

?>
--EXPECT--
object(stdClass)#1 (7) {
  ["result"]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (4) {
      ["_key"]=>
      string(5) "70614"
      ["_id"]=>
      string(14) "products/70614"
      ["_rev"]=>
      string(11) "_al3hU1K---"
      ["Hello3"]=>
      string(6) "World3"
    }
    [1]=>
    object(stdClass)#3 (4) {
      ["_key"]=>
      string(5) "70616"
      ["_id"]=>
      string(14) "products/70616"
      ["_rev"]=>
      string(11) "_al3hU1K--A"
      ["Hello4"]=>
      string(6) "World4"
    }
  }
  ["hasMore"]=>
  bool(false)
  ["count"]=>
  int(2)
  ["cached"]=>
  bool(false)
  ["extra"]=>
  object(stdClass)#4 (2) {
    ["stats"]=>
    object(stdClass)#5 (8) {
      ["writesExecuted"]=>
      int(0)
      ["writesIgnored"]=>
      int(0)
      ["scannedFull"]=>
      int(4)
      ["scannedIndex"]=>
      int(0)
      ["filtered"]=>
      int(0)
      ["httpRequests"]=>
      int(0)
      ["executionTime"]=>
      float(0.00014734268188476562)
      ["peakMemoryUsage"]=>
      int(2558)
    }
    ["warnings"]=>
    array(0) {
    }
  }
  ["error"]=>
  bool(false)
  ["code"]=>
  int(201)
}
array(7) {
  ["result"]=>
  array(2) {
    [0]=>
    array(4) {
      ["_key"]=>
      string(5) "70614"
      ["_id"]=>
      string(14) "products/70614"
      ["_rev"]=>
      string(11) "_al3hU1K---"
      ["Hello3"]=>
      string(6) "World3"
    }
    [1]=>
    array(4) {
      ["_key"]=>
      string(5) "70616"
      ["_id"]=>
      string(14) "products/70616"
      ["_rev"]=>
      string(11) "_al3hU1K--A"
      ["Hello4"]=>
      string(6) "World4"
    }
  }
  ["hasMore"]=>
  bool(false)
  ["count"]=>
  int(2)
  ["cached"]=>
  bool(false)
  ["extra"]=>
  array(2) {
    ["stats"]=>
    array(8) {
      ["writesExecuted"]=>
      int(0)
      ["writesIgnored"]=>
      int(0)
      ["scannedFull"]=>
      int(4)
      ["scannedIndex"]=>
      int(0)
      ["filtered"]=>
      int(0)
      ["httpRequests"]=>
      int(0)
      ["executionTime"]=>
      float(0.00014734268188476562)
      ["peakMemoryUsage"]=>
      int(2558)
    }
    ["warnings"]=>
    array(0) {
    }
  }
  ["error"]=>
  bool(false)
  ["code"]=>
  int(201)
}
