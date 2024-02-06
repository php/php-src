--TEST--
Bug #76675 (Segfault with H2 server push write/writeheader handlers)
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
$transfers = 1;
$callback = function($parent, $passed) use (&$transfers) {
    curl_setopt($passed, CURLOPT_WRITEFUNCTION, function ($ch, $data) {
        echo "Received ".strlen($data);
        return strlen($data);
    });
    $transfers++;
    return CURL_PUSH_OK;
};
$mh = curl_multi_init();
curl_multi_setopt($mh, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
curl_multi_setopt($mh, CURLMOPT_PUSHFUNCTION, $callback);
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, (getenv('SERVER_NAME') ?: 'https://localhost') . '/serverpush');
curl_setopt($ch, CURLOPT_HTTP_VERSION, 3);
curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_multi_add_handle($mh, $ch);
$active = null;
do {
    $status = curl_multi_exec($mh, $active);
    do {
        $info = curl_multi_info_read($mh);
        if (false !== $info && $info['msg'] == CURLMSG_DONE) {
            $handle = $info['handle'];
            if ($handle !== null) {
                $transfers--;
                curl_multi_remove_handle($mh, $handle);
                curl_close($handle);
            }
        }
    } while ($info);
} while ($transfers);
curl_multi_close($mh);
?>
--EXPECTREGEX--
(Received \d+)+
