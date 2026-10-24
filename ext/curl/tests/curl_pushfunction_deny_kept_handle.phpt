--TEST--
CURLMOPT_PUSHFUNCTION denied handle kept by the callback can be reset, copied and cloned
--EXTENSIONS--
curl
--SKIPIF--
<?php
include 'skipif-nocaddy.inc';

$curl_version = curl_version();
if ($curl_version['version_number'] < 0x080100) {
    exit("skip: test may crash with curl < 8.1.0");
}
?>
--FILE--
<?php
$kept = null;
$callback = function ($parent_ch, $pushed_ch, array $headers) use (&$kept) {
    $kept = $pushed_ch;
    return CURL_PUSH_DENY;
};

$mh = curl_multi_init();
curl_multi_setopt($mh, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
curl_multi_setopt($mh, CURLMOPT_PUSHFUNCTION, $callback);

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "https://localhost/serverpush");
curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_multi_add_handle($mh, $ch);

$done = false;
do {
    curl_multi_exec($mh, $active);
    do {
        $info = curl_multi_info_read($mh);
        if (false !== $info && $info['msg'] == CURLMSG_DONE) {
            curl_multi_remove_handle($mh, $info['handle']);
            $done = true;
        }
    } while ($info);
} while (!$done);

curl_multi_close($mh);

curl_reset($kept);
var_dump(curl_copy_handle($kept));
try {
    clone $kept;
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
Warning: curl_copy_handle(): Cannot duplicate cURL handle in %s on line %d
bool(false)
Exception: Failed to clone CurlHandle
