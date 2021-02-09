--TEST--
Bug #78719 (http wrapper silently ignores long Location headers)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$url = str_repeat('*', 1024);
$responses = array(
	"data://text/plain,HTTP/1.0 302 Ok\r\nLocation: $url\r\n\r\nBody",
	"data://text/plain,HTTP/1.0 302 Ok\r\nLocation: $url$url\r\n\r\nBody",
);

$pid = http_server("tcp://127.0.0.1:12342", $responses, $output);

function test() {
    $context = stream_context_create(['http' => ['follow_location' => 0]]);
    $f = file_get_contents('http://127.0.0.1:12342/', false, $context);
    var_dump($f);
}
test();
test();

http_server_kill($pid);
?>
--EXPECTF--
string(4) "Body"

Warning: file_get_contents(http://127.0.0.1:12342/): failed to open stream: Location header too long in %s on line %d
bool(false)
