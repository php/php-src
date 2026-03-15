--TEST--
Poll stream - only wait
--FILE--
<?php
require_once __DIR__ . '/poll.inc';
$poll_ctx = pt_new_stream_poll();
$events = $poll_ctx->wait(0, 100000);
pt_print_events($events);

?>
--EXPECT--
Events count: 0
