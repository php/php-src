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
--EXPECT--
Done

Fatal error: Registered shutdown function self::on_shutdown() cannot be called, function does not exist in Unknown on line 0
