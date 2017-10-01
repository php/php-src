--TEST--
array curl_multi_info_read ( resource $mh [, int &$msgs_in_queue = NULL ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) { die('skip: online test'); }
if (!extension_loaded('curl')) { print("skip"); }
?>
--FILE--
<?php
$urls = array(
    "bugs.php.net",
    "pear.php.net"
);

$mh = curl_multi_init();
foreach ($urls as $i => $url) {
    $conn[$i] = curl_init($url);
    curl_setopt($conn[$i], CURLOPT_RETURNTRANSFER, 1);
    curl_multi_add_handle($mh, $conn[$i]);
}

do {
    $status = curl_multi_exec($mh, $active);
    $info = curl_multi_info_read($mh);
    if (false !== $info) {
        var_dump(is_array($info));
    }
} while ($status === CURLM_CALL_MULTI_PERFORM || $active);

foreach ($urls as $i => $url) {
    curl_close($conn[$i]);
}
?>
--EXPECT--
bool(true)
bool(true)
