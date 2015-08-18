--TEST--
Test basic functionality of flush()
--FILE--
<?php
/* 
 * proto void flush(void)
 * Function is implemented in ext/standard/basic_functions.c.
 */
  
// Verify return type
var_dump(flush());

// Ensure user buffers are not flushed by flush()
ob_start();
echo "Inside a user buffer\n";
flush();
ob_end_clean();

echo "Outside of any user buffers\n";
var_dump(flush());

?>
--EXPECT--
NULL
Outside of any user buffers
NULL
