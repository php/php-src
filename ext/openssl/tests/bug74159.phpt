--TEST--
Bug #74159: Writing a large buffer to non-blocking encrypted streams fails
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
if (OPENSSL_VERSION_NUMBER < 0x10001001) die("skip OpenSSLv1.0.1 required");
?>
--FILE--
<?php
// the server code is doing many readings in a short interval which is
// not really reliable on more powerful machine but cover different
// scenarios which might be useful. More reliable test is bug72333.phpt
$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:10012";
	$serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
	$serverCtx = stream_context_create(['ssl' => [
	    'local_cert' => __DIR__ . '/bug54992.pem',
		'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_SERVER,
	]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
	phpt_notify();

    $client = stream_socket_accept($server, 1);

    if (!$client) {
	    exit();
	}

    $data = '';
	while (strlen($data) < 0xfffff) {
	    $buffer = fread($client, 8192);
		if (empty($buffer)) {
		    exit();
		}
		$data .= $buffer;
		usleep(100);
	}

    fclose($client);
CODE;

$clientCode = <<<'CODE'
    function streamRead($stream) : int {
	    return strlen(fread($stream, 8192));
	}

    function streamWrite($stream, $data) : int {
	    return fwrite($stream, $data);
	}

    function waitForWrite(...$streams) : bool {
	    $read = null;
		$except = null;
		while($streams && !($n = stream_select($read, $streams, $except, 1)));
		return $n > 0;
	}

    function waitForRead(...$streams) : bool {
	    $write = null;
		$except = null;
		while ($streams && !($n = stream_select($streams, $write, $except, 1)));
		return $n > 0;
	}

    set_error_handler(function ($errno, $errstr) {
	    exit("$errstr\n");
	});

    $serverUri = "tcp://127.0.0.1:10012";
	$clientFlags = STREAM_CLIENT_CONNECT;
	$clientCtx = stream_context_create(['ssl' => [
	    'verify_peer' => true,
		'cafile' => __DIR__ . '/bug54992-ca.pem',
		'peer_name' => 'bug54992.local',
	]]);

    phpt_wait();

    $fp = stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx);

    stream_set_blocking($fp, false);
	while (0 === ($n = stream_socket_enable_crypto($fp, true, STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT)));

    $data = str_repeat("a", 0xfffff);
	$written = 0;
	$total = $written;
	while(!empty($data)) {
	    $written = streamWrite($fp, $data);
		$total += $written;
		$data = substr($data, $written);
		waitForWrite($fp);
	}
	printf("Written %d bytes\n", $total);

    while(waitForRead($fp)) {
	    streamRead($fp);
		if (feof($fp)) {
		    break;
		}
	}

    exit("DONE\n");
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
Written 1048575 bytes
DONE
