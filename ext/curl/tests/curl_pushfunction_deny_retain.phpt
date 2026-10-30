--TEST--
CURLMOPT_PUSHFUNCTION deny with the pushed handle retained by the callback
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
$kept = [];
$callback = function ($parent_ch, $pushed_ch, array $headers) use (&$kept) {
    $kept[] = $pushed_ch;
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
unset($ch, $info);
gc_collect_cycles();

var_dump(count($kept));
var_dump(curl_setopt($kept[0], CURLOPT_WRITEFUNCTION, fn($h, $data) => strlen($data)));
unset($kept);
gc_collect_cycles();
echo "Done\n";
?>
--EXPECT--
int(1)
bool(true)
Done
