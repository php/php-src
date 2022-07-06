--TEST--
http:// and ignore_errors
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

function do_test($context_options) {

    $context = stream_context_create(array('http' => $context_options));

    $responses = array(
        "data://text/plain,HTTP/1.1 200 Ok\r\nX-Foo: bar\r\n\r\n1",
        "data://text/plain,HTTP/1.1 404 Not found\r\nX-bar: baz\r\n\r\n2",
    );

    ['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

    foreach($responses as $r) {

        $fd = fopen("$uri/foo/bar", 'rb', false, $context);
        var_dump($fd);

        if ($fd) {
            $meta_data = stream_get_meta_data($fd);
            var_dump($meta_data['wrapper_data']);

            var_dump(stream_get_contents($fd));
        }

        fseek($output, 0, SEEK_SET);
        var_dump(stream_get_contents($output));
        fseek($output, 0, SEEK_SET);
    }

    http_server_kill($pid);
}

echo "-- Test: requests without ignore_errors --\n";

do_test(array());

echo "-- Test: requests with ignore_errors --\n";

do_test(array('ignore_errors' => true));

echo "-- Test: requests with ignore_errors (2) --\n";

do_test(array('ignore_errors' => 1));

?>
--EXPECTF--
-- Test: requests without ignore_errors --
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(15) "HTTP/1.1 200 Ok"
  [1]=>
  string(10) "X-Foo: bar"
}
string(1) "1"
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"

Warning: fopen(http://%s:%d/foo/bar): Failed to open stream: HTTP request failed! HTTP/1.1 404 Not found
 in %s on line %d
bool(false)
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
-- Test: requests with ignore_errors --
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(15) "HTTP/1.1 200 Ok"
  [1]=>
  string(10) "X-Foo: bar"
}
string(1) "1"
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(22) "HTTP/1.1 404 Not found"
  [1]=>
  string(10) "X-bar: baz"
}
string(1) "2"
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
-- Test: requests with ignore_errors (2) --
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(15) "HTTP/1.1 200 Ok"
  [1]=>
  string(10) "X-Foo: bar"
}
string(1) "1"
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(22) "HTTP/1.1 404 Not found"
  [1]=>
  string(10) "X-bar: baz"
}
string(1) "2"
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
