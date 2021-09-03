--TEST--
socket_import_stream: effects of leaked handles
--EXTENSIONS--
sockets
zend_test
--INI--
report_memleaks=0
--FILE--
<?php

$stream0 = stream_socket_server("udp://0.0.0.0:0", $errno, $errstr, STREAM_SERVER_BIND);
$sock0 = socket_import_stream($stream0);
zend_leak_variable($stream0);

$stream1 = stream_socket_server("udp://0.0.0.0:0", $errno, $errstr, STREAM_SERVER_BIND);
$sock1 = socket_import_stream($stream1);
zend_leak_variable($sock1);

echo "Done.\n";
?>
--EXPECT--
Done.
