--TEST--
CURLMOPT_PUSHFUNCTION denied handle kept by the callback is a fresh handle
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
curl_setopt($ch, CURLOPT_PRIVATE, "parent");
curl_multi_add_handle($mh, $ch);

do {
    curl_multi_exec($mh, $active);
    $info = curl_multi_info_read($mh);
} while ($info === false || $info['msg'] !== CURLMSG_DONE);

curl_multi_remove_handle($mh, $ch);
unset($mh, $ch, $info);

var_dump($kept instanceof CurlHandle);
var_dump(curl_getinfo($kept, CURLINFO_PRIVATE));
var_dump(curl_pause($kept, CURLPAUSE_CONT) === curl_pause(curl_init(), CURLPAUSE_CONT));

var_dump(curl_setopt($kept, CURLOPT_URL, "https://localhost/"));
var_dump(curl_exec($kept));
var_dump(curl_getinfo($kept, CURLINFO_RESPONSE_CODE));

curl_reset($kept);
curl_setopt($kept, CURLOPT_URL, "https://localhost/");
curl_setopt($kept, CURLOPT_RETURNTRANSFER, true);
var_dump(curl_exec($kept));

$copy = curl_copy_handle($kept);
var_dump(curl_exec($copy));

$clone = clone $kept;
var_dump(curl_exec($clone));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(true)
Caddy is up and runningbool(true)
int(200)
string(23) "Caddy is up and running"
string(23) "Caddy is up and running"
string(23) "Caddy is up and running"
