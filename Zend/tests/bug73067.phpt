--TEST--
Bug #73067 (__debugInfo crashes when throwing an exception)
--FILE--
<?php

class Debug
{
	public function __debugInfo()
	{
		throw new Exception("Test Crash");
	}
}

try {
	var_dump(new Debug());
} catch (Exception $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
Fatal error: __debuginfo() must return an array in %sbug73067.php on line %d
