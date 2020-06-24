--TEST--
Test ob_implicit_flush() function : ensure implicit flushing does not apply to user buffers.
--FILE--
<?php
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
--EXPECT--
*** Testing ob_implicit_flush() : ensure implicit flushing does not apply to user buffers. ***
Done
