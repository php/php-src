--TEST--
stream context tcp_nodelay fopen
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
if (!extension_loaded("sockets")) die("skip: need sockets");
?>
--FILE--
<?php
$ctxt = stream_context_create([
	"socket" => [
		"tcp_nodelay" => true
	],
	"http" => [
		"follow_location" => 0
	]
]);

$stream = fopen("http://www.php.net", "r", false,  $ctxt);

$socket =
	@socket_import_stream($stream);

var_dump(socket_get_option($socket, STREAM_IPPROTO_TCP, TCP_NODELAY) > 0);
?>
--EXPECT--
bool(true)
