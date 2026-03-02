--TEST--
Test ob_flush() function : basic functionality
--FILE--
<?php
echo "*** Testing ob_flush() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing ob_flush() function with Zero arguments --\n";
var_dump(ob_flush());

ob_start();
echo "This should get flushed.\n";
var_dump(ob_flush());

echo "Ensure the buffer is still active after the flush.\n";
$out = ob_flush();
var_dump($out);

echo "Done";

?>
--EXPECTF--
*** Testing ob_flush() : basic functionality ***

-- Testing ob_flush() function with Zero arguments --

Notice: ob_flush(): Failed to flush buffer. No buffer to flush in %s on line %d
bool(false)
This should get flushed.
bool(true)
Ensure the buffer is still active after the flush.
bool(true)
Done
