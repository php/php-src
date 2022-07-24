--TEST--
is_json() tests
--INI--
memory_limit=-1
--FILE--
<?php

/*
try {
  var_dump(is_json('{ : {} "foo": "bar" }, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'));
} catch (Exception $error) {
  var_dump($error->getMessage());
}
return;

var_dump(is_json('{ "test": {} "foo": "bar" }, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'));
return;
*/

/*
echo "-----------------------\n";
$limit = 1000000;
$jsonString = '{ "test": { "foo": "bar" },';

for ($i=0; $i < $limit; $i++) {
  $jsonString .= " \"test$i\": { \"foo\": { \"test\" : { \"foo\" :  { \"test\" : { \"foo\" : \"bar\" }}}}},";
}

$jsonString .= ' "testXXXX": { "foo": "replaceme" } }';
//{ "test" : { "foo" : "bar" }}}

$memoryBefore = memory_get_usage(true) / 1024 / 1024;
$memoryBeforePeak = memory_get_peak_usage(true) / 1024 / 1024;
echo "Megas used before call: " . $memoryBefore . PHP_EOL;
echo "PEAK Megas used before call: " . $memoryBeforePeak . PHP_EOL;

$start = microtime(true);

//json_decode($jsonString, null, $limit, 0);
var_dump(is_json($jsonString));

$memoryAfter = memory_get_usage(true) / 1024 / 1024;
$memoryAfterPeak = memory_get_peak_usage(true) / 1024 / 1024;
echo "Megas used after call: " . $memoryAfter . PHP_EOL;
echo "PEAK Megas used after call: " . $memoryAfterPeak . PHP_EOL;

echo "Difference: " . ($memoryAfter - $memoryBefore) . PHP_EOL;
echo "PEAK Difference: " . ($memoryAfterPeak - $memoryBeforePeak) . PHP_EOL;

echo "Time: " . (microtime(true) - $start) . " seconds" . PHP_EOL;
return;
*/



var_dump(json_decode(""), is_json(""));
echo "-------------------\n";
var_dump(json_decode("."), is_json("."));
echo "-------------------\n";
var_dump(json_decode("<?>"), is_json("<?>"));
echo "-------------------\n";
var_dump(json_decode(";"), is_json(";"));
echo "-------------------\n";
var_dump(json_decode("руссиш"), is_json("руссиш"));
echo "-------------------\n";
var_dump(json_decode("blah"), is_json("blah"));
echo "-------------------\n";
var_dump(json_decode(NULL), is_json(NULL));
echo "-------------------\n";
var_dump(json_decode('{ "test": { "foo": "bar" } }'), is_json('{ "test": { "foo": "bar" } }'));
echo "-------------------\n";
var_dump(json_decode('{ "test": { "foo": "" } }'), is_json('{ "test": { "foo": "" } }'));
echo "-------------------\n";
var_dump(json_decode('{ "": { "foo": "" } }'), is_json('{ "": { "foo": "" } }'));
echo "-------------------\n";
var_dump(json_decode('{ "": { "": "" } }'), is_json('{ "": { "": "" } }'));
echo "-------------------\n";
var_dump(json_decode('{ "": { "": "" }'), is_json('{ "": { "": "" }'));
echo "-------------------\n";
var_dump(json_decode('{ "": "": "" } }'), is_json('{ "": "": "" } }'));
?>

--EXPECTF--
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------

Deprecated: json_decode(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d

Deprecated: is_json(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d
NULL
bool(false)
-------------------
object(stdClass)#2 (1) {
  ["test"]=>
  object(stdClass)#1 (1) {
    ["foo"]=>
    string(3) "bar"
  }
}
bool(true)
-------------------
object(stdClass)#1 (1) {
  ["test"]=>
  object(stdClass)#2 (1) {
    ["foo"]=>
    string(0) ""
  }
}
bool(true)
-------------------
object(stdClass)#2 (1) {
  [""]=>
  object(stdClass)#1 (1) {
    ["foo"]=>
    string(0) ""
  }
}
bool(true)
-------------------
object(stdClass)#1 (1) {
  [""]=>
  object(stdClass)#2 (1) {
    [""]=>
    string(0) ""
  }
}
bool(true)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)