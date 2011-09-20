<?php
define ("PHP_CLI_SERVER_ADDRESS", "localhost:8964");

function php_cli_server_start($php_executable, $code = 'echo "Hello world";') {
	$doc_root = __DIR__;
	$router = "router.php";
	file_put_contents($doc_root . '/' . $router, '<?php ' . $code . ' ?>');

	$descriptorspec = array(
		0 => STDIN,
		1 => STDOUT,
		2 => STDERR,
	);

	$cmd = "{$php_executable} -t {$doc_root} -S " . PHP_CLI_SERVER_ADDRESS . " {$router}";

	$handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root);
	sleep(1);

	return $handle;
}

function php_cli_server_shutdown($handle) {
	proc_terminate($handle);
	proc_close($handle);
	@unlink(__DIR__ . "router.php");
	return true;
}
?>
