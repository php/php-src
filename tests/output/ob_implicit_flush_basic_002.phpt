--TEST--
Test ob_implicit_flush() function : ensure implicit flushing does not apply to user buffers.
--FILE--
<?php
/* Prototype  : proto void ob_implicit_flush([int flag])
 * Description: Turn implicit flush on/off and is equivalent to calling flush() after every output call 
 * Source code: main/output.c
 * Alias to functions: 
 */

echo "*** Testing ob_implicit_flush() : ensure implicit flushing does not apply to user buffers. ***\n";

// Start a user buffer
ob_start();
// Switch on implicit flushing.
ob_implicit_flush(1);

echo "This is being written to a user buffer.\n";
echo "Note that even though implicit flushing is on, you should never see this,\n";
echo "because implicit flushing affects only the top level buffer, not user buffers.\n";

// Wipe the user buffer. Nothing should have been flushed.
ob_end_clean();

echo "Done";
?>
--EXPECTF--
*** Testing ob_implicit_flush() : ensure implicit flushing does not apply to user buffers. ***
Done