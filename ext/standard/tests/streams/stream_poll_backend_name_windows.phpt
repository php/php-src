--TEST--
Stream polling - backend name on Windows
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die ("skip only for Windows");
}
?>
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';
// select is always available
$poll_ctx = stream_poll_create(STREAM_POLL_BACKEND_WSAPOLL);
var_dump(stream_poll_backend_name($poll_ctx));
// test with string
$poll_ctx = stream_poll_create('wsapoll');
var_dump(stream_poll_backend_name($poll_ctx));

?>
--EXPECT--
string(7) "wsapoll"
string(7) "wsapoll"
