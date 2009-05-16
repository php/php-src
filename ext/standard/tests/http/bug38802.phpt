--TEST--
Bug #38802 (ignore_errors and max_redirects)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

function do_test($context_options) {

	$context = stream_context_create(array('http' => $context_options));

	$responses = array(
		"data://text/plain,HTTP/1.0 302 Moved Temporarily\r\nLocation: http://127.0.0.1:12342/foo/bar2\r\n\r\n1",
		"data://text/plain,HTTP/1.0 301 Moved Permanently\r\nLocation: http://127.0.0.1:12342/foo/bar3\r\n\r\n",
		"data://text/plain,HTTP/1.0 302 Moved Temporarily\r\nLocation: http://127.0.0.1:12342/foo/bar4\r\n\r\n3",
		"data://text/plain,HTTP/1.0 200 OK\r\n\r\ndone.",
	);

	$pid = http_server("tcp://127.0.0.1:12342", $responses, $output);

	$fd = fopen('http://127.0.0.1:12342/foo/bar', 'rb', false, $context);
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
  string(30) "HTTP/1.0 302 Moved Temporarily"
  [1]=>
  string(41) "Location: http://127.0.0.1:12342/foo/bar2"
  [2]=>
  string(30) "HTTP/1.0 301 Moved Permanently"
  [3]=>
  string(41) "Location: http://127.0.0.1:12342/foo/bar3"
  [4]=>
  string(30) "HTTP/1.0 302 Moved Temporarily"
  [5]=>
  string(41) "Location: http://127.0.0.1:12342/foo/bar4"
  [6]=>
  string(15) "HTTP/1.0 200 OK"
}
string(5) "done."
string(195) "GET /foo/bar HTTP/1.0
Host: 127.0.0.1:12342

GET /foo/bar2 HTTP/1.0
Host: 127.0.0.1:12342

GET /foo/bar3 HTTP/1.0
Host: 127.0.0.1:12342

GET /foo/bar4 HTTP/1.0
Host: 127.0.0.1:12342

"
-- Test: fail after 2 redirections --

Warning: fopen(http://127.0.0.1:12342/foo/bar): failed to open stream: Redirection limit reached, aborting in %s
bool(false)
string(97) "GET /foo/bar HTTP/1.0
Host: 127.0.0.1:12342

GET /foo/bar2 HTTP/1.0
Host: 127.0.0.1:12342

"
-- Test: fail at first redirection --

Warning: fopen(http://127.0.0.1:12342/foo/bar): failed to open stream: Redirection limit reached, aborting in %s
bool(false)
string(48) "GET /foo/bar HTTP/1.0
Host: 127.0.0.1:12342

"
-- Test: fail at first redirection (2) --

Warning: fopen(http://127.0.0.1:12342/foo/bar): failed to open stream: Redirection limit reached, aborting in %s
bool(false)
string(48) "GET /foo/bar HTTP/1.0
Host: 127.0.0.1:12342

"
-- Test: return at first redirection --
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(30) "HTTP/1.0 302 Moved Temporarily"
  [1]=>
  string(41) "Location: http://127.0.0.1:12342/foo/bar2"
}
string(1) "1"
string(48) "GET /foo/bar HTTP/1.0
Host: 127.0.0.1:12342

"
-- Test: return at first redirection (2) --
resource(%d) of type (stream)
array(2) {
  [0]=>
  string(30) "HTTP/1.0 302 Moved Temporarily"
  [1]=>
  string(41) "Location: http://127.0.0.1:12342/foo/bar2"
}
string(1) "1"
string(48) "GET /foo/bar HTTP/1.0
Host: 127.0.0.1:12342

"
-- Test: return at second redirection --
resource(%d) of type (stream)
array(4) {
  [0]=>
  string(30) "HTTP/1.0 302 Moved Temporarily"
  [1]=>
  string(41) "Location: http://127.0.0.1:12342/foo/bar2"
  [2]=>
  string(30) "HTTP/1.0 301 Moved Permanently"
  [3]=>
  string(41) "Location: http://127.0.0.1:12342/foo/bar3"
}
string(0) ""
string(97) "GET /foo/bar HTTP/1.0
Host: 127.0.0.1:12342

GET /foo/bar2 HTTP/1.0
Host: 127.0.0.1:12342

"
