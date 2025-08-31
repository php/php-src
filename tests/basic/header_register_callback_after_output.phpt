--TEST--
Call header_register_callback() after headers sent
--FILE--
<?php
echo "Send headers.\n";
header_register_callback(function() { echo "Too late";});
?>
--EXPECT--
Send headers.
