--TEST--
Bug #67351 (copy() should handle HTTP 304 response)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:22346'); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
	"data://text/plain,HTTP/1.0 304 Not Modified\r\n\r\n",
	"data://text/plain,HTTP/1.0 304 Not Modified\r\n\r\n",
);

$pid = http_server("tcp://127.0.0.1:22346", $responses, $output);

function test() {
    $options = [
        'http' => [
            'method' => 'GET',
            'protocol_version' => '1.1',
            'header' => "If-Modified-Since: Tue, 19 Jan 2038 03:14:07 GMT\r\n"
        ]
    ];
    var_dump(file_get_contents('http://127.0.0.1:22346/', false, stream_context_create($options)));
    var_dump(copy('http://127.0.0.1:22346/', __FILE__ . '.txt', stream_context_create($options)));
}
test();

http_server_kill($pid);
?>
--EXPECTF--
Warning: file_get_contents(http://127.0.0.1:22346/): failed to open stream: HTTP request failed! HTTP/1.0 304 Not Modified
 in %s on line %d
bool(false)

Warning: copy(http://127.0.0.1:22346/): failed to open stream: HTTP request failed! HTTP/1.0 304 Not Modified
 in %s on line %d
bool(false)
