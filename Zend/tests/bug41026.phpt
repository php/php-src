--TEST--
Bug #41026 (segfault when calling "self::method()" in shutdown functions)
--FILE--
<?php

class try_class
{
	static public function main ()
	{
		register_shutdown_function(array ("self", "on_shutdown"));
	}

	static public function on_shutdown ()
	{
		printf ("CHECKPOINT\n"); /* never reached */
	}
}

try_class::main();
echo "Done\n";
?>
--EXPECT--
Done

Fatal error: Uncaught ValueError: (Registered shutdown functions) Unable to call self::on_shutdown() - function does not exist in [no active file]:0
Stack trace:
#0 {main}
  thrown in [no active file] on line 0
