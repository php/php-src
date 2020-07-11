--TEST--
Test ob_clean() function : basic functionality
--FILE--
<?php
echo "*** Testing ob_clean() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing ob_clean() function with Zero arguments --\n";
var_dump( ob_clean() );

ob_start();
echo "You should never see this.";
var_dump(ob_clean());

echo "Ensure the buffer is still active after the clean.";
$out = ob_get_clean();
var_dump($out);

echo "Done";
?>
--EXPECTF--
*** Testing ob_clean() : basic functionality ***

-- Testing ob_clean() function with Zero arguments --

Notice: ob_clean(): Failed to delete buffer. No buffer to delete in %s on line %d
bool(false)
string(61) "bool(true)
Ensure the buffer is still active after the clean."
Done
