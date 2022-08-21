--TEST--
is_json() - Tests is_json() performance
--INI--
memory_limit=128M
--FILE--
<?php

//Generate a JSON STring
$limit = 20000;
$jsonString = '{ "test": { "foo": "bar" },';
for ($i=0; $i < $limit; $i++) {
  $jsonString .= " \"test$i\": { \"foo\": { \"test\" : { \"foo\" :  { \"test\" : { \"foo\" : \"bar\" }}}}},";
}
$jsonString .= ' "testXXXX": { "foo": "replaceme" } }';

//Start json_decode() test

$memoryBefore = memory_get_usage(true) / 1024 / 1024;
$memoryBeforePeak = memory_get_peak_usage(true) / 1024 / 1024;
$start = microtime(true);

json_decode($jsonString, true, $limit, 0);

$memoryAfter = memory_get_usage(true) / 1024 / 1024;
$memoryAfterPeak = memory_get_peak_usage(true) / 1024 / 1024;

$json_decode_memory_usage = ($memoryAfter - $memoryBefore);
$json_decode_memory_peak_usage = ($memoryAfterPeak - $memoryBeforePeak);
$json_decode_time = (microtime(true) - $start);

unset($memoryBefore);
unset($memoryBeforePeak);
unset($start);
unset($memoryAfter);
unset($memoryAfterPeak);

//Start is_json() tests

$memoryBefore = memory_get_usage(true) / 1024 / 1024;
$memoryBeforePeak = memory_get_peak_usage(true) / 1024 / 1024;
$start = microtime(true);

is_json($jsonString);

$memoryAfter = memory_get_usage(true) / 1024 / 1024;
$memoryAfterPeak = memory_get_peak_usage(true) / 1024 / 1024;

$is_json_memory_usage = ($memoryAfter - $memoryBefore);
$is_json_memory_peak_usage = ($memoryAfterPeak - $memoryBeforePeak);
$is_json_time = (microtime(true) - $start);


echo "is_json() uses less memory than json_decode() => {$is_json_memory_usage} MB < {$json_decode_memory_usage} MB = ". (($is_json_memory_usage < $json_decode_memory_usage) ? "TRUE" : "FALSE") . PHP_EOL;
echo "is_json() uses less memory peak than json_decode() => {$is_json_memory_peak_usage} MB < {$json_decode_memory_peak_usage} MB = ". (($is_json_memory_peak_usage < $json_decode_memory_peak_usage) ? "TRUE" : "FALSE") . PHP_EOL;
echo "is_json() is faster than json_decode() => {$is_json_time} Seconds < {$json_decode_time} Seconds = ". (((float)$is_json_time < (float)$json_decode_time) ? "TRUE" : "FALSE") . PHP_EOL;

return;

?>
--EXPECTF--
is_json() uses less memory than json_decode() => %d MB < %d MB = TRUE
is_json() uses less memory peak than json_decode() => %d MB < %d MB = TRUE
is_json() is faster than json_decode() => %f Seconds < %f Seconds = TRUE
