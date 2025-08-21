--TEST--
Stream polling basic functionality - only wait
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';
$poll_ctx = new_stream_poll();
$events = stream_poll_wait($poll_ctx, 0);
var_dump(is_array($events));
echo "Events count: " . count($events) . "\n";

?>
--EXPECT--
bool(true)
Events count: 0
