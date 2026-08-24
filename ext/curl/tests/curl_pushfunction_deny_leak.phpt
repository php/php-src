--TEST--
CURLMOPT_PUSHFUNCTION deny frees the pushed handle
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
$pushed_path = null;
$pushed_ref = null;
$callback = function ($parent_ch, $pushed_ch, array $headers) use (&$pushed_path, &$pushed_ref) {
    $pushed_path = preg_grep('/^:path:/', $headers);
    $pushed_ref = WeakReference::create($pushed_ch);
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

do {
    curl_multi_exec($mh, $active);
    $info = curl_multi_info_read($mh);
} while ($info === false || $info['msg'] !== CURLMSG_DONE);

var_dump(curl_multi_getcontent($ch));
var_dump(array_values($pushed_path));
var_dump($pushed_ref->get());
?>
--EXPECT--
string(13) "main response"
array(1) {
  [0]=>
  string(24) ":path:/serverpush/pushed"
}
NULL
