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
		2 => STDERR,
	);

	if (substr(PHP_OS, 0, 3) == 'WIN') {
		$cmd = "{$php_executable} -t {$doc_root} -n -S " . PHP_CLI_SERVER_ADDRESS;
		if (!$no_router) {
			$cmd .= " {$router}";
		}

		$handle = proc_open(addslashes($cmd), $descriptorspec, $pipes, $doc_root, NULL, array("bypass_shell" => true,  "suppress_errors" => true));
	} else {
		$cmd = "exec {$php_executable} -t {$doc_root} -n -S " . PHP_CLI_SERVER_ADDRESS;
		if (!$no_router) {
			$cmd .= " {$router}";
		}
		$cmd .= " 2>/dev/null";

		$handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root);
	}

	register_shutdown_function(
		function($handle) use($router) {
			proc_terminate($handle);
			@unlink(__DIR__ . "/{$router}");
		},
			$handle
		);
	usleep(50000);
}
?>

