--TEST--
Exception inside a foreach loop with on an object with destructor
--FILE--
<?php
class bar {
	public $foo = 1;
	public $bar = 2;
	function __destruct() {
		throw new Exception("test");
	}
}
function foo(): string {
	foreach (new bar() as $foo) {
		try {
			$foo = new Exception;
			return;
		} catch (Exception $e) {
			echo "Exception1: " . $e->getMessage() . "\n";
		} catch (Error $e) {
			echo "Error1: " . $e->getMessage() . "\n";
		}
	}
	echo "bag!\n";
}
try {
	foo();
} catch (Throwable $e) {
	echo (($e instanceof Exception) ? "Exception2: " : "Error2: ") .
		$e->getMessage() . "\n";
	$e = $e->getPrevious();
	while ($e instanceof Throwable) {
		echo "\tPrev " . (($e instanceof Exception) ? "Exception2: " : "Error2: ") .
			$e->getMessage() . "\n";
		$e = $e->getPrevious();		
	}
}
echo "ok\n";
?>
--EXPECTF--
Exception2: test
	Prev Error2: Return value of foo() must be of the type string, none returned in %stemporary_cleaning_008.php on line %d
ok
