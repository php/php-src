--TEST--
Bug #41026 (segfault when calling "self::method()" in shutdown functions)
--FILE--
<?php

class try_class
{
	static public function main ()
	{
		register_shutdown_function (array ("self", "on_shutdown"));
	}

	static public function on_shutdown ()
	{
		printf ("CHECKPOINT\n"); /* never reached */
	}
}

try_class::main ();

echo "Done\n";
?>
--EXPECTF--
Done

Warning: (Registered shutdown functions) Unable to call self::on_shutdown() - function does not exist in Unknown on line 0
