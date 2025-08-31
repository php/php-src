--TEST--
Bug #43510 (stream_get_meta_data() does not return same mode as used in fopen)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.0 200 OK\r\n\r\n",
    "data://text/plain,HTTP/1.0 200 OK\r\n\r\n",
);

['pid' => $pid, 'uri' => $uri ] = http_server($responses, $output);

foreach(array('r', 'rb') as $mode) {
    $fd = fopen($uri, $mode, false);
    $meta = stream_get_meta_data($fd);
    var_dump($meta['mode']);
    fclose($fd);
}

http_server_kill($pid);

?>
--EXPECT--
string(1) "r"
string(2) "rb"
