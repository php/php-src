--TEST--
Stream polling - only wait
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';
$poll_ctx = pt_new_stream_poll();
$events = stream_poll_wait($poll_ctx, 0);
pt_print_events($events);

?>
--EXPECT--
Events count: 0
