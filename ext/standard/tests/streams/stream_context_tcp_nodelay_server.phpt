--TEST--
stream context tcp_nodelay server
--SKIPIF--
<?php if (!extension_loaded("sockets")) die("skip: need sockets") ?>
--FILE--
<?php
$serverCode = <<<'CODE'
   $ctxt = stream_context_create([
		"socket" => [
			"tcp_nodelay" => true
		]
	]);

	$server = stream_socket_server(
		"tcp://127.0.0.1:9099", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctxt);

	$client = stream_socket_accept($server);

	var_dump(socket_get_option(
				socket_import_stream($server), 
					SOL_TCP, TCP_NODELAY) > 0);

	var_dump(socket_get_option(
				socket_import_stream($client), 
					SOL_TCP, TCP_NODELAY) > 0);

	fclose($client);
	fclose($server);
CODE;

$clientCode = <<<'CODE'
    $test = stream_socket_client(
		"tcp://127.0.0.1:9099", $errno, $errstr, 10);

	sleep(1);

	fclose($test);
CODE;

include sprintf(
	"%s/../../../openssl/tests/ServerClientTestCase.inc", 
	dirname(__FILE__));
ServerClientTestCase::getInstance()->run($serverCode, $clientCode);
?>
--EXPECT--
bool(false)
bool(true)
