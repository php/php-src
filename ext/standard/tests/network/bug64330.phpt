--TEST--
Bug #64330 (stream_socket_server() creates wrong Abstract Namespace UNIX sockets)
--SKIPIF--
<?php
if (!in_array("unix", stream_get_transports())) die("SKIP unix domain sockets unavailable");
if (!stristr(PHP_OS, "Linux")) die('skip abstract namespace is a Linux-only feature');
?>
--FILE--
<?php
echo "Test\n";
$server = stream_socket_server("unix://\x00/MyBindName");
$client = stream_socket_client("unix://\x00/MyBindName");
if ($client) {
	echo "ok\n";
}
?>
===DONE===
--EXPECT--
Test
ok
===DONE===
