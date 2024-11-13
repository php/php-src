--TEST--
GH-14643: Segfault on empty user function.
--FILE--
<?php
class Logger {
	public function __construct() {
		register_shutdown_function(function () {
			$this->flush();
			register_shutdown_function([$this, 'flush'], true);
		});
	}
	public function flush($final = false) {
	}
}
while (true) {
	$a = new Logger();
}
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s

Fatal error: Allowed memory size of %d bytes exhausted %s
