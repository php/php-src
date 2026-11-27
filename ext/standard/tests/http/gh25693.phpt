--TEST--
GH-25693 (Use-after-free of the last response headers in a stream wrapper closed at shutdown)
--CREDITS--
xuziqiang98
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

class Wrapper {
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }

    public function stream_close() {
        var_dump(http_get_last_response_headers());
    }
}

function request($uri) {
    var_dump(file_get_contents($uri));
}

['pid' => $pid, 'uri' => $uri] = http_server([
    "data://text/plain,HTTP/1.0 200 Ok\r\n\r\nBody",
]);

request($uri);

http_server_kill($pid);

stream_wrapper_register('test', Wrapper::class);
$fp = fopen('test://', 'r');
?>
--EXPECT--
string(4) "Body"
NULL
