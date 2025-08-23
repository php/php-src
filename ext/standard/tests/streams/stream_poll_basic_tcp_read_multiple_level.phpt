--TEST--
Stream polling - TCP read write level
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($clients, $servers) = pt_new_tcp_socket_connections(20);
$poll_ctx = pt_new_stream_poll();

for ($i = 0; $i < count($servers); $i++) {
    stream_poll_add($poll_ctx, $servers[$i], STREAM_POLL_READ, "server{$i}_data");
}

pt_print_events(stream_poll_wait($poll_ctx, 0));
for ($i = 0; $i < count($clients); $i++) {
    pt_write_sleep($clients[$i], "test $i data");
}
pt_print_events(stream_poll_wait($poll_ctx, 100), true);
pt_write_sleep($clients[1], "more data");
pt_write_sleep($clients[2], "more data");
pt_print_events(stream_poll_wait($poll_ctx, 100), true);
pt_print_events(stream_poll_wait($poll_ctx, 100), true);

?>
--EXPECT--
Events count: 0
Events count: 20
Event[0]: 1, user data: server0_data, read data: 'test 0 data'
Event[1]: 1, user data: server1_data, read data: 'test 1 data'
Event[2]: 1, user data: server2_data, read data: 'test 2 data'
Event[3]: 1, user data: server3_data, read data: 'test 3 data'
Event[4]: 1, user data: server4_data, read data: 'test 4 data'
Event[5]: 1, user data: server5_data, read data: 'test 5 data'
Event[6]: 1, user data: server6_data, read data: 'test 6 data'
Event[7]: 1, user data: server7_data, read data: 'test 7 data'
Event[8]: 1, user data: server8_data, read data: 'test 8 data'
Event[9]: 1, user data: server9_data, read data: 'test 9 data'
Event[10]: 1, user data: server10_data, read data: 'test 10 data'
Event[11]: 1, user data: server11_data, read data: 'test 11 data'
Event[12]: 1, user data: server12_data, read data: 'test 12 data'
Event[13]: 1, user data: server13_data, read data: 'test 13 data'
Event[14]: 1, user data: server14_data, read data: 'test 14 data'
Event[15]: 1, user data: server15_data, read data: 'test 15 data'
Event[16]: 1, user data: server16_data, read data: 'test 16 data'
Event[17]: 1, user data: server17_data, read data: 'test 17 data'
Event[18]: 1, user data: server18_data, read data: 'test 18 data'
Event[19]: 1, user data: server19_data, read data: 'test 19 data'
Events count: 2
Event[0]: 1, user data: server1_data, read data: 'more data'
Event[1]: 1, user data: server2_data, read data: 'more data'
Events count: 0
