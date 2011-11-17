<?php
define ("PHP_CLI_SERVER_ADDRESS", "localhost:8964");

function php_cli_server_start($code = 'echo "Hello world";', $no_router = FALSE) {
	$php_executable = getenv('TEST_PHP_EXECUTABLE');
	$doc_root = __DIR__;
	$router = "index.php";
	if ($code) {
		file_put_contents($doc_root . '/' . $router, '<?php ' . $code . ' ?>');
	}

	$descriptorspec = array(
		0 => STDIN,
		1 => STDOUT,
		2 => array("pipe", "w"),
	);

	if (substr(PHP_OS, 0, 3) == 'WIN') {
		$cmd = "{$php_executable} -t {$doc_root} -S " . PHP_CLI_SERVER_ADDRESS;
		if (!$no_router) {
			$cmd .= " {$router}";
		}

		$handle = proc_open(addslashes($cmd), $descriptorspec, $pipes, $doc_root, NULL, array("bypass_shell" => true));
	} else {
		$cmd = "exec {$php_executable} -t {$doc_root} -S " . PHP_CLI_SERVER_ADDRESS;
		if (!$no_router) {
			$cmd .= " {$router}";
		}

		$handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root);
	}

	//@FIXME is there a better way to make sure the process is ready?
	usleep(50000);

	stream_set_blocking($pipes[2], 0);
	if ($err = stream_get_contents($pipes[2])) {
		fclose($pipes[2]);
		proc_terminate($handle);
		@unlink(__DIR__ . "/{$router}");
		die("Cli sever could not be started: " . $err);
	}

	register_shutdown_function(
		function($handle, $router, $pipes) {
			stream_get_contents($pipes[2]);
			fclose($pipes[2]);
			proc_terminate($handle);
			@unlink(__DIR__ . "/{$router}");
		},
			$handle, $router, $pipes
		);
}
?>
