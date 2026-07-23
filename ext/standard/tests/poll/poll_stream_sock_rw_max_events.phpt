--TEST--
Poll stream - socket read/write events with maxEvents equal to the FD count
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$poll_ctx = pt_new_stream_poll();

// Each FD is ready for both read and write. Backends that report read and
// write separately (kqueue) see twice as many events as FDs and have to merge
// them without dropping any FD, filling only half of the maxEvents entries.
$writers = [];
$expected = [];
for ($i = 0; $i < 4; $i++) {
    list($reader, $writer) = pt_new_socket_pair();
    $writers[$i] = $writer;
    pt_stream_poll_add($poll_ctx, $reader, [Io\Poll\Event::Read, Io\Poll\Event::Write], "sock$i");
    pt_write_sleep($writer, "test $i data");
    $expected[] = [
        'events' => [Io\Poll\Event::Read, Io\Poll\Event::Write],
        'data' => "sock$i",
        'read' => "test $i data",
    ];
}

pt_expect_events($poll_ctx->wait(0, 100000, 8), $expected);

// All read data was drained above, so only write events remain
$expected = [];
for ($i = 0; $i < 4; $i++) {
    $expected[] = ['events' => [Io\Poll\Event::Write], 'data' => "sock$i"];
}
pt_expect_events($poll_ctx->wait(0, 100000, 8), $expected);

?>
--EXPECT--
Events matched - count: 4
Events matched - count: 4
