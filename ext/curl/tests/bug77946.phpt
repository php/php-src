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
    'http://nonexistent.tld',
);

$mh = curl_multi_init();
$connMulti = array();
$connSingle = array();

foreach ($urls as $url) {
    // multi-curl
    $h = curl_init($url);

    curl_multi_add_handle($mh, $h);
    // Use the reource reference as index for both arrays
    $index = (int) $h;
    $connMulti[$index] = $h;

    // single-curl
    $ch = curl_init($url);
    $connSingle[$index] = $ch;
}

do {
    $status = curl_multi_exec($mh, $active);
    $info = curl_multi_info_read($mh);
    if (false !== $info) {
        $index = (int) $info['handle'];
        echo '"result" offset from curl_multi_info_read(): '.$info['result']."\n";
        echo 'curl_multi_errno() from multi handle: '.curl_multi_errno($mh)."\n";
        echo 'curl_errno() from multi handle: '.curl_errno($connMulti[$index])."\n";
        echo 'curl_error() from multi handle: '.curl_error($connMulti[$index])."\n";

        curl_exec($connSingle[$index]);

        echo 'curl_errno() from single handle: '.curl_errno($connSingle[$index])."\n";
        echo 'curl_error() from single handle: '.curl_error($connSingle[$index])."\n";

        curl_close($connMulti[$index]);
        curl_close($connSingle[$index]);
    }
} while ($active || CURLM_CALL_MULTI_PERFORM === $status);

curl_multi_close($mh);
?>
--EXPECT--
"result" offset from curl_multi_info_read(): 6
curl_multi_errno() from multi handle: 6
curl_errno() from multi handle: 6
curl_error() from multi handle: Could not resolve host: nonexistent.tld
curl_errno() from single handle: 6
curl_error() from single handle: Could not resolve host: nonexistent.tld
