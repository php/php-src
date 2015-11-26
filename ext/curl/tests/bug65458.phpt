--TEST--
Bug #65458 (curl memory leak)
--SKIPIF--
<?php
if (!extension_loaded('curl')) exit("skip curl extension not loaded");
?>
--FILE--
<?php
$ch = curl_init();
$init = memory_get_usage();
for ($i = 0; $i < 10000; $i++) {
    curl_setopt($ch, CURLOPT_HTTPHEADER, [ "SOAPAction: getItems" ]);
}

$preclose = memory_get_usage();
curl_close($ch);

// This is a slightly tricky heuristic, but basically, we want to ensure
// $preclose - $init has a delta in the order of bytes, not megabytes. Given
// the number of iterations in the loop, if we're wasting memory here, we
// should have megs and megs of extra allocations.
var_dump(($preclose - $init) < 10000);
?>
--EXPECT--
bool(true)
