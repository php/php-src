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
        printf ("CHECKPOINT\n");
    }
}

try_class::main ();

echo "Done\n";
?>
--EXPECTF--
Deprecated: Use of "self" in callables is deprecated in %s on line %d
Done
CHECKPOINT
