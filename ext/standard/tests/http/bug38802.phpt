--TEST--
Bug #38802 (ignore_errors and max_redirects)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
require 'server.inc'; http_server_skipif();
?>
--FILE--
<?php
require 'server.inc';

function genResponses($server) {
    $uri = 'http://' . stream_socket_get_name($server, false);
    yield "data://text/plain,HTTP/1.1 302 Moved Temporarily\r\nLocation: $uri/foo/bar2\r\n\r\n1";
    yield "data://text/plain,HTTP/1.1 301 Moved Permanently\r\nLocation: $uri/foo/bar3\r\n\r\n";
    yield "data://text/plain,HTTP/1.1 302 Moved Temporarily\r\nLocation: $uri/foo/bar4\r\n\r\n3";
    yield "data://text/plain,HTTP/1.1 200 OK\r\n\r\ndone.";
}

function do_test($context_options) {

    $context = stream_context_create(array('http' => $context_options));

	$uri = null;
    ['pid' => $pid, 'uri' => $uri ] = http_server('genResponses', $output);

    $fd = fopen("$uri/foo/bar", 'rb', false, $context);
    var_dump($fd);

    if ($fd) {
        $meta_data = stream_get_meta_data($fd);
        var_dump($meta_data['wrapper_data']);

        var_dump(stream_get_contents($fd));
    }

    fseek($output, 0, SEEK_SET);
    var_dump(stream_get_contents($output));

    http_server_kill($pid);
}

echo "-- Test: follow all redirections --\n";

do_test(array(), 4);

echo "-- Test: fail after 2 redirections --\n";

do_test(array('max_redirects' => 2), 2);

echo "-- Test: fail at first redirection --\n";

do_test(array('max_redirects' => 0), 1);

echo "-- Test: fail at first redirection (2) --\n";

do_test(array('max_redirects' => 1), 1);

echo "-- Test: return at first redirection --\n";

do_test(array('max_redirects' => 0, 'ignore_errors' => 1), 1);

echo "-- Test: return at first redirection (2) --\n";

do_test(array('max_redirects' => 1, 'ignore_errors' => 1), 1);

echo "-- Test: return at second redirection --\n";

do_test(array('max_redirects' => 2, 'ignore_errors' => 1), 2);

?>
--EXPECTF--
-- Test: follow all redirections --
resource(%d) of type (stream)
array(7) {
  [0]=>
  string(30) "HTTP/1.1 302 Moved Temporarily"
  [1]=>
  string(%d) "Location: http://%s:%d/foo/bar2"
  [2]=>
  string(30) "HTTP/1.1 301 Moved Permanently"
  [3]=>
  string(%d) "Location: http://%s:%d/foo/bar3"
  [4]=>
  string(30) "HTTP/1.1 302 Moved Temporarily"
  [5]=>
  string(%d) "Location: http://%s:%d/foo/bar4"
  [6]=>
  string(15) "HTTP/1.1 200 OK"
}
string(5) "done."
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

GET /foo/bar2 HTTP/1.1
Host: %s:%d
Connection: close

GET /foo/bar3 HTTP/1.1
Host: %s:%d
Connection: close

GET /foo/bar4 HTTP/1.1
Host: %s:%d
Connection: close

"
-- Test: fail after 2 redirections --

Warning: fopen(http://%s:%d/foo/bar): Failed to open stream: Redirection limit reached, aborting in %s
bool(false)
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

GET /foo/bar2 HTTP/1.1
Host: %s:%d
Connection: close

"
-- Test: fail at first redirection --

Warning: fopen(http://%s:%d/foo/bar): Failed to open stream: Redirection limit reached, aborting in %s
bool(false)
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
-- Test: fail at first redirection (2) --

Warning: fopen(http://%s:%d/foo/bar): Failed to open stream: Redirection limit reached, aborting in %s
bool(false)
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
-- Test: return at first redirection --
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(30) "HTTP/1.1 302 Moved Temporarily"
  [1]=>
  string(%d) "Location: http://%s:%d/foo/bar2"
}
string(1) "1"
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
-- Test: return at first redirection (2) --
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(30) "HTTP/1.1 302 Moved Temporarily"
  [1]=>
  string(%d) "Location: http://%s:%d/foo/bar2"
}
string(1) "1"
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

"
-- Test: return at second redirection --
resource(%d) of type (stream)
array(4) {
  [0]=>
  string(30) "HTTP/1.1 302 Moved Temporarily"
  [1]=>
  string(%d) "Location: http://%s:%d/foo/bar2"
  [2]=>
  string(30) "HTTP/1.1 301 Moved Permanently"
  [3]=>
  string(%d) "Location: http://%s:%d/foo/bar3"
}
string(0) ""
string(%d) "GET /foo/bar HTTP/1.1
Host: %s:%d
Connection: close

GET /foo/bar2 HTTP/1.1
Host: %s:%d
Connection: close

"
