--TEST--
Bug - socket_export_stream() with wrong protocol
--EXTENSIONS--
sockets
--FILE--
<?php
$sock = socket_create(AF_INET, SOCK_DGRAM, 0);
$stream = socket_export_stream($sock);
echo stream_get_meta_data($stream)['stream_type']. "\n";
?>
--EXPECT--
udp_socket
