<?php
define ("PHP_CLI_SERVER_HOSTNAME", "localhost");
define ("PHP_CLI_SERVER_PORT", 8964);
define ("PHP_CLI_SERVER_ADDRESS", PHP_CLI_SERVER_HOSTNAME.":".PHP_CLI_SERVER_PORT);

function php_cli_server_start($code = 'echo "Hello world";', $router = 'index.php', $cmd_args = null) {
	$php_executable = getenv('TEST_PHP_EXECUTABLE');
	$doc_root = __DIR__;
    $error = null;

	if ($code) {
		file_put_contents($doc_root . '/' . ($router ?: 'index.php'), '<?php ' . $code . ' ?>');
	}

	if (substr(PHP_OS, 0, 3) == 'WIN') {
		$descriptorspec = array(
			0 => STDIN,
			1 => STDOUT,
			2 => array("pipe", "w"),
		);

		$cmd = "{$php_executable} -t {$doc_root} -n {$cmd_args} -S " . PHP_CLI_SERVER_ADDRESS;
		if (!is_null($router)) {
			$cmd .= " {$router}";
		}

		$handle = proc_open(addslashes($cmd), $descriptorspec, $pipes, $doc_root, NULL, array("bypass_shell" => true,  "suppress_errors" => true));
	} else {
		$descriptorspec = array(
			0 => STDIN,
			1 => STDOUT,
			2 => STDERR,
		);

		$cmd = "exec {$php_executable} -t {$doc_root} -n {$cmd_args} -S " . PHP_CLI_SERVER_ADDRESS;
		if (!is_null($router)) {
			$cmd .= " {$router}";
		}
		$cmd .= " 2>/dev/null";

		$handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root);
	}

    // note: here we check the process is running
    for ($i=0; $i < 120; $i++) {
        $status = proc_get_status($handle);

        if (!$status || !$status['running']) {
            if ($status &&
               ($status['running'] == false && $status['exitcode'] != 0)) {
                $error =
                    "Server could not be started\n";
                break;
            }

            usleep(50000); // 50ms per try
            continue;
        }

        if ($status['signaled']) {
            $error =
                "Server was terminated with {$status['termsig']}\n";
            break;
        }

        if ($status['stopped']) {
            $error =
                "Server was stopped with {$status['stopsig']}\n";
            break;
        }

        // note: here we check the server is listening, even when the server prints
        //          listening on %s:%d
        //       it may not be ready to accept connections
        $start = time();

        for ($try = 0; $try < 120; $try++) {
            $error = @fsockopen(
                        PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT) ?
                            null :
                            sprintf(
                                "Server is not accepting connections after %d seconds\n",
                                time() - $start);

            if (!$error) {
                break 2;
            }

            usleep(50000);
        }

        break;
    }

php_cli_server_start_error:
    if ($error) {
        echo $error;
        proc_terminate($handle);
        exit(1);
    }

    register_shutdown_function(
        function($handle) use($router) {
            proc_terminate($handle);
            @unlink(__DIR__ . "/{$router}");
        },
        $handle
    );

    return $handle;
}

function php_cli_server_stop($handle) {
    $success = FALSE;
    if ($handle) {
        proc_terminate($handle);
        /* Wait for server to shutdown */
        for ($i = 0; $i < 60; $i++) {
            $status = proc_get_status($handle);
            if (!($status && $status['running'])) {
                $success = TRUE;
                break;
            }
            usleep(50000);
        }
    }
    return $success;
}
?>
