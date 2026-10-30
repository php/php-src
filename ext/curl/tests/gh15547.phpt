--TEST--
GH-15547 - curl_multi_select overflow on timeout argument
--EXTENSIONS--
curl
--FILE--
<?php

$mh = curl_multi_init();

try {
	curl_multi_select($mh, -2500000);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
curl_multi_close($mh);
$mh = curl_multi_init();
try {
	curl_multi_select($mh, 2500000);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
curl_multi_close($mh);
$mh = curl_multi_init();
var_dump(curl_multi_select($mh, 1000000));
?>
--EXPECTF--
curl_multi_select(): Argument #2 ($timeout) must be between %d and %d
curl_multi_select(): Argument #2 ($timeout) must be between %d and %d
int(0)
