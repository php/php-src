--TEST--
Test header_register_callback
--FILE--
<?php
header_register_callback(function() { echo "sent";});
?>
--EXPECT--
sent
