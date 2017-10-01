<?php
define ("PHP_CLI_SERVER_HOSTNAME", "localhost");
define ("PHP_CLI_SERVER_PORT", 8964);
define ("PHP_CLI_SERVER_ADDRESS", PHP_CLI_SERVER_HOSTNAME.":".PHP_CLI_SERVER_PORT);

function php_cli_server_start($ini = "") {
	$php_executable = getenv('TEST_PHP_EXECUTABLE');
	$doc_root = __DIR__;

	$descriptorspec = array(
		0 => STDIN,
		1 => STDOUT,
		2 => STDERR,
	);

	if (substr(PHP_OS, 0, 3) == 'WIN') {
		$cmd = "{$php_executable} -t {$doc_root} $ini -S " . PHP_CLI_SERVER_ADDRESS;
		$handle = proc_open(addslashes($cmd), $descriptorspec, $pipes, $doc_root, NULL, array("bypass_shell" => true,  "suppress_errors" => true));
	} else {
		$cmd = "exec {$php_executable} -t {$doc_root} $ini -S " . PHP_CLI_SERVER_ADDRESS . " 2>/dev/null";
		$handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root);
	}

	// note: even when server prints 'Listening on localhost:8964...Press Ctrl-C to quit.'
	//       it might not be listening yet...need to wait until fsockopen() call returns
	$error = "Unable to connect to server\n";
	for ($i=0; $i < 60; $i++) {
		usleep(50000); // 50ms per try
		$status = proc_get_status($handle);
		$fp = @fsockopen(PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT);
		// Failure, the server is no longer running
		if (!($status && $status['running'])) {
			$error = "Server is not running\n";
			break;
		}
		// Success, Connected to servers
		if ($fp) {
			$error = '';
			break;
		}
	}

	if ($fp) {
		fclose($fp);
	}

	if ($error) {
		echo $error;
		proc_terminate($handle);
		exit(1);
	}

	register_shutdown_function(
		function($handle) {
			proc_terminate($handle);
			/* Wait for server to shutdown */
			for ($i = 0; $i < 60; $i++) {
				$status = proc_get_status($handle);
				if (!($status && $status['running'])) {
					break;
				}
			usleep(50000);
			}
		},
		$handle
	);

}
?>

