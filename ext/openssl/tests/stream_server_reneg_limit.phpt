--TEST--
TLS server rate-limits client-initiated renegotiation
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!function_exists('pcntl_fork')) die("skip no fork");
exec('openssl help', $out, $code);
if ($code > 0) die("skip couldn't locate openssl binary");
--FILE--
<?php

/**
 * This test uses the openssl binary directly to initiate renegotiation. At this time it's not
 * possible renegotiate the TLS handshake in PHP userland, so using the openssl s_client binary
 * command is the only feasible way to test renegotiation limiting functionality. It's not an ideal
 * solution, but it's really the only way to get test coverage on the rate-limiting functionality
 * given current limitations.
 */

$bindTo = 'ssl://127.0.0.1:12345';
$flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
$server = stream_socket_server($bindTo, $errNo, $errStr, $flags, stream_context_create(['ssl' => [
	'local_cert' => __DIR__ . '/bug54992.pem',
	'reneg_limit' => 0,
	'reneg_window' => 30,
	'reneg_limit_callback' => function($stream) {
		var_dump($stream);
	}
]]));

$pid = pcntl_fork();
if ($pid == -1) {
	die('could not fork');
} elseif ($pid) {

	$cmd = 'openssl s_client -connect 127.0.0.1:12345';
	$descriptorspec = array(
		0 => array("pipe", "r"),
		1 => array("pipe", "w"),
		2 => array("pipe", "w"),
	);
	$process = proc_open($cmd, $descriptorspec, $pipes);

	list($stdin, $stdout, $stderr) = $pipes;

	// Trigger renegotiation twice
	// Server settings only allow one per second (should result in disconnection)
	fwrite($stdin, "R\nR\nR\nR\n");

	$lines = [];
	while(!feof($stderr)) {
		fgets($stderr);
	}

	fclose($stdin);
	fclose($stdout);
	fclose($stderr);
	proc_terminate($process);
	pcntl_wait($status);

} else {

	$clients = [];

	while (1) {
		$r = array_merge([$server], $clients);
		$w = $e = [];

		stream_select($r, $w, $e, $timeout=42);

		foreach ($r as $sock) {
			if ($sock === $server && ($client = stream_socket_accept($server, $timeout = 42))) {
				$clientId = (int) $client;
				$clients[$clientId] = $client;
			} elseif ($sock !== $server) {
				$clientId = (int) $sock;
				$buffer = fread($sock, 1024);
				if (strlen($buffer)) {
					continue;
				} elseif (!is_resource($sock) || feof($sock)) {
					unset($clients[$clientId]);
					break 2;
				}
			}
		}
	}
}
--EXPECTF--
resource(%d) of type (stream)
