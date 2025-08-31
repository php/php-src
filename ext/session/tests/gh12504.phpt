--TEST--
GH-12504 (Corrupted session written when there's a fatal error in autoloader)
--EXTENSIONS--
session
--FILE--
<?php

class TestSessionHandler implements SessionHandlerInterface
{
	public function close(): bool
	{
		return true;
	}
	public function destroy(string $id): bool
	{
		return true;
	}
	public function gc(int $max_lifetime): int|false
	{
		return 0;
	}
	public function open(string $path, string $name): bool
	{
		return true;
	}
	public function read(string $id): string|false
	{
		// Return a session object that has 3 variables
		return 'before|i:1234;test|O:4:"Test":0:{}after|i:5678;';
	}
	public function write($id, $data): bool
	{
		echo 'Write session:' . PHP_EOL;
		echo $data . PHP_EOL;
		return true;
	}
}

register_shutdown_function(function() {
	echo "In shutdown function\n";
	var_dump($_SESSION);
});

session_set_save_handler(new TestSessionHandler());

// Autoload class that's in session
spl_autoload_register(function() {
	// Easiest way to reproduce the issue is to dynamically define a class with a bogus definition
	eval('class Test {public int $var = null;}');
	return true;
});

session_start();

?>
--EXPECTF--
Fatal error: Default value for property of type int may not be null. Use the nullable type ?int to allow null default value in %s on line %d

Warning: Unknown: Failed to decode session object. Session has been destroyed in Unknown on line 0
In shutdown function
array(0) {
}
