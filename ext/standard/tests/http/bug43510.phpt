--TEST--
Bug #43510 (stream_get_meta_data() does not return same mode as used in fopen)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

$responses = array(
	"data://text/plain,HTTP/1.0 200 OK\r\n\r\n",
	"data://text/plain,HTTP/1.0 200 OK\r\n\r\n",
);

$pid = http_server("tcp://127.0.0.1:12342", $responses, $output);

foreach(array('r', 'rb') as $mode) {
	$fd = fopen('http://127.0.0.1:12342/', $mode, false);
	$meta = stream_get_meta_data($fd);
	var_dump($meta['mode']);
	fclose($fd);
}

http_server_kill($pid);

?>
--EXPECT--
string(1) "r"
string(2) "rb"
