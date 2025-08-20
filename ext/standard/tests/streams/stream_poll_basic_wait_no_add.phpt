--TEST--
Stream polling basic functionality - only wait
--SKIPIF--
<?php
if (!function_exists('stream_poll_create')) {
    die("skip stream polling not available");
}
?>
--FILE--
<?php
$poll_ctx = stream_poll_create();
$events = stream_poll_wait($poll_ctx, 0);
var_dump(is_array($events));
echo "Events count: " . count($events) . "\n";

?>
--EXPECT--
bool(true)
Events count: 0
