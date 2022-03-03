--TEST--
Bug #78719 (http wrapper silently ignores long Location headers)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$url = str_repeat('*', 2000);
$responses = array(
	"data://text/plain,HTTP/1.0 302 Ok\r\nLocation: $url\r\n\r\nBody",
);
['pid' => $pid, 'uri' => $uri] = http_server($responses);

$context = stream_context_create(['http' => ['follow_location' => 0]]);
$stream = fopen($uri, 'r', false, $context);
var_dump(stream_get_contents($stream));
var_dump(stream_get_meta_data($stream)['wrapper_data'][1] === "Location: $url");

http_server_kill($pid);
?>
--EXPECT--
string(4) "Body"
bool(true)
