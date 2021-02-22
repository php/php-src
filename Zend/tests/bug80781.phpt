--TEST--
Bug #80781: Error handler that throws ErrorException infinite loop
--FILE--
<?php

function handle(int $severity, string $message, string $file, int $line): bool {
	if((error_reporting() & $severity) !== 0) {
		throw new \ErrorException($message, 0, $severity, $file, $line);
	}

	return true; // stfu operator
}

set_error_handler('handle');

function getPlugin(string $plugin) : bool{
	return false;
}

$data = [];
$array = [];
if (isset($array[$data]) or getPlugin($data)) {

}

?>
--EXPECTF--
Fatal error: Uncaught ErrorException: Illegal offset type in isset or empty in %s:%d
Stack trace:
#0 %s(%d): handle(2, 'Illegal offset ...', %s, %d, Array)
#1 {main}
  thrown in %s on line %d
