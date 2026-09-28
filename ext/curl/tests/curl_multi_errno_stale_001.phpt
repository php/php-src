--TEST--
curl_multi_errno() must not stay stale after curl_multi_select() or a failed CURLMOPT_PUSHFUNCTION setopt
--EXTENSIONS--
curl
--FILE--
<?php

$mh = curl_multi_init();
$ch = curl_init();
curl_multi_add_handle($mh, $ch);
curl_multi_add_handle($mh, $ch); // CURLM_ADDED_ALREADY
echo curl_multi_errno($mh), "\n";

curl_multi_select($mh, 0.01);
echo curl_multi_errno($mh), "\n";

curl_multi_add_handle($mh, $ch); // CURLM_ADDED_ALREADY again
echo curl_multi_errno($mh), "\n";

try {
    curl_multi_setopt($mh, CURLMOPT_PUSHFUNCTION, "not a callable");
} catch (TypeError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
echo curl_multi_errno($mh), "\n";

?>
--EXPECTF--
7
0
7
TypeError: curl_multi_setopt(): Argument #2 ($option) must be a valid callback for option CURLMOPT_PUSHFUNCTION, %s
10
