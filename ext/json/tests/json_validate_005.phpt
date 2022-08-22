--TEST--
json_validate() - Tests json_validate() performance
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

//Start json_validate() tests

$memoryBefore = memory_get_usage(true) / 1024 / 1024;
$memoryBeforePeak = memory_get_peak_usage(true) / 1024 / 1024;
$start = microtime(true);

json_validate($jsonString);

$memoryAfter = memory_get_usage(true) / 1024 / 1024;
$memoryAfterPeak = memory_get_peak_usage(true) / 1024 / 1024;

$json_validate_memory_usage = ($memoryAfter - $memoryBefore);
$json_validate_memory_peak_usage = ($memoryAfterPeak - $memoryBeforePeak);
$json_validate_time = (microtime(true) - $start);


echo "json_validate() uses less memory than json_decode() => {$json_validate_memory_usage} MB < {$json_decode_memory_usage} MB = ". (($json_validate_memory_usage < $json_decode_memory_usage) ? "TRUE" : "FALSE") . PHP_EOL;
echo "json_validate() uses less memory peak than json_decode() => {$json_validate_memory_peak_usage} MB < {$json_decode_memory_peak_usage} MB = ". (($json_validate_memory_peak_usage < $json_decode_memory_peak_usage) ? "TRUE" : "FALSE") . PHP_EOL;
echo "json_validate() is faster than json_decode() => {$json_validate_time} Seconds < {$json_decode_time} Seconds = ". (((float)$json_validate_time < (float)$json_decode_time) ? "TRUE" : "FALSE") . PHP_EOL;

return;

?>
--EXPECTF--
json_validate() uses less memory than json_decode() => %d MB < %d MB = TRUE
json_validate() uses less memory peak than json_decode() => %d MB < %d MB = TRUE
json_validate() is faster than json_decode() => %f Seconds < %f Seconds = TRUE