--TEST--
Bug #77946 (Errored cURL resources returned by curl_multi_info_read() must be compatible with curl_errno() and curl_error())
--SKIPIF--
<?php

if (!extension_loaded('curl')) {
	exit('skip curl extension not loaded');
}

?>
--FILE--
<?php
$urls = array(
   'unknown://scheme.tld',
);

$mh = curl_multi_init();

foreach ($urls as $i => $url) {
    $conn[$i] = curl_init($url);
    curl_multi_add_handle($mh, $conn[$i]);
}

do {
    $status = curl_multi_exec($mh, $active);
    $info = curl_multi_info_read($mh);
    if (false !== $info) {
        var_dump($info['result']);
        var_dump(curl_errno($info['handle']));
        var_dump(curl_error($info['handle']));
    }
} while ($status === CURLM_CALL_MULTI_PERFORM || $active);

foreach ($urls as $i => $url) {
    curl_close($conn[$i]);
}

curl_multi_close($mh);
?>
--EXPECTF--
int(1)
int(1)
string(%d) "Protocol %Sunknown%S not supported or disabled in libcurl"
