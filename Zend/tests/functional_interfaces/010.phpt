--TEST--
functional interfaces: scope/this
--FILE--
<?php
interface ILog {
	public function log(string $message, ...$args);
}

class Foo {

	public function __construct() {
		$this->stream = fopen("php://stdout", "w");	
	}

	public function getLogger() : ILog {
		return function (string $message, ... $args) implements ILog {
			fprintf($this->stream, $message, ... $args);
		};
	}

	protected $stream;
}

$foo = new Foo();
$logger = 
	$foo->getLogger();
$logger->log("php7 baby\n");
?>
--EXPECTF--
php7 baby
