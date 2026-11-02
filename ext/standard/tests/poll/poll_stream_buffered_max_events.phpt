--TEST--
Io\Poll: buffered data reports count against maxEvents
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$a, $b] = pt_new_socket_pair();
[$c, $d] = pt_new_socket_pair();
[$e, $f] = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
pt_stream_poll_add($ctx, $a, [Io\Poll\Event::Read], 'a');
pt_stream_poll_add($ctx, $c, [Io\Poll\Event::Read], 'c');
pt_stream_poll_add($ctx, $e, [Io\Poll\Event::Read], 'e');

fwrite($b, "first\nsecond\n");
fwrite($d, "first\nsecond\n");
var_dump(fgets($a), fgets($c));
fwrite($f, "x"); // on the socket, not buffered

var_dump(count($ctx->wait(Time\Duration::fromSeconds(0), 1)));
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0), 2)));
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0), 3)));

var_dump(fgets($a), fgets($c), fread($e, 10));
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));
?>
--EXPECT--
string(6) "first
"
string(6) "first
"
int(1)
int(2)
int(3)
int(3)
string(7) "second
"
string(7) "second
"
string(1) "x"
int(0)
