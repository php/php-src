--TEST--
Test ob_get_level() function : basic functionality
--FILE--
<?php
echo "*** Testing ob_get_level() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing ob_get_level() function with Zero arguments --\n";
var_dump(ob_get_level());

ob_start();
var_dump(ob_get_level());

ob_start();
var_dump(ob_get_level());

ob_end_flush();
var_dump(ob_get_level());

ob_end_flush();
var_dump(ob_get_level());

ob_end_flush();
var_dump(ob_get_level());


echo "Done";
?>
--EXPECTF--
*** Testing ob_get_level() : basic functionality ***

-- Testing ob_get_level() function with Zero arguments --
int(0)
int(1)
int(2)
int(1)
int(0)

Notice: ob_end_flush(): Failed to delete and flush buffer. No buffer to delete or flush in %s on line %d
int(0)
Done
