--TEST--
Request boundary releases the request state set between two requests
--EXTENSIONS--
zend_test
session
--INI--
display_errors=0
session.use_cookies=0
session.cache_limiter=
--FILE--
<?php
function temp_streams(): int
{
    return count(array_filter(get_resources('stream'), static fn ($s): bool => stream_get_meta_data($s)['stream_type'] === 'TEMP'));
}

$streams = temp_streams();
$memory = memory_get_usage();
$session = [];

zend_test_request_boundary(static function () use (&$session): void {
    header('X-Between: ' . str_repeat('a', 1 << 20));
    header('HTTP/1.1 200 ' . str_repeat('a', 1 << 20));
    header('Content-Type: text/' . str_repeat('a', 1 << 20));
    fclose(fopen('php://input', 'r'));
    output_add_rewrite_var('between', 'requests');

    $session[] = session_id('between');
    $session[] = error_get_last()['message'];
    $session[] = session_start();
    $session[] = error_get_last()['message'];
});

var_dump(memory_get_usage() - $memory < 1 << 20);
var_dump(temp_streams() === $streams);
var_dump($session, session_id());

output_add_rewrite_var('current', 'request');
echo '<form action="/next"></form>', "\n";
?>
--EXPECT--
bool(true)
bool(true)
array(4) {
  [0]=>
  bool(false)
  [1]=>
  string(68) "session_id(): Session ID cannot be changed when no request is active"
  [2]=>
  bool(false)
  [3]=>
  string(68) "session_start(): Session cannot be started when no request is active"
}
string(0) ""
<form action="/next"><input type="hidden" name="current" value="request" /></form>
