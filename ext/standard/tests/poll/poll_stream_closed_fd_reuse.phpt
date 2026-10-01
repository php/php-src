--TEST--
Io\Poll: a new stream reusing the fd number of a closed watched stream can be added
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

$new = $ctx->add(new StreamPollHandle($r1), [Io\Poll\Event::Read], "new");
var_dump($old->isActive());
var_dump($new->isActive());

fwrite($w1, "ping");
$events = $ctx->wait(Time\Duration::fromMicroseconds(100000));
var_dump(count($events));
var_dump($events[0] === $new);
var_dump($events[0]->getData());

$old->remove();
echo "old removed\n";

$new->remove();
var_dump($new->isActive());
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
?>
--EXPECT--
bool(false)
bool(true)
int(1)
bool(true)
string(3) "new"
old removed
bool(false)
Events count: 0
