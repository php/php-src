--TEST--
Io\Poll: an unwatched stream reusing the fd number of a closed watched stream is never reported
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN') {
    die("skip fd numbers are not reused predictably on Windows\n");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = pt_new_stream_poll();

list($r0, $w0) = pt_new_socket_pair();
$old = $ctx->add(new StreamPollHandle($r0), [Io\Poll\Event::Read], "old");

// The lowest free fd number is handed out first, so $r1 takes the number $r0 had
fclose($r0);
list($r1, $w1) = pt_new_socket_pair();
fwrite($w1, "ping");

echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
var_dump($old->isActive());

$old->remove();
var_dump($old->isActive());
var_dump(fread($r1, 10));
?>
--EXPECT--
Events count: 0
Events count: 0
bool(false)
bool(false)
string(4) "ping"
