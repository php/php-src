--TEST--
Stream polling - backend name
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';
// select is always available
$poll_ctx = stream_poll_create(STREAM_POLL_BACKEND_SELECT);
var_dump(stream_poll_backend_name($poll_ctx));
// test with string
$poll_ctx = stream_poll_create('select');
var_dump(stream_poll_backend_name($poll_ctx));

?>
--EXPECT--
string(6) "select"
string(6) "select"
