--TEST--
register_shutdown_function() & closure
--FILE--
<?php
register_shutdown_function(function () { echo "Hello World!\n"; });

echo "Done\n";
?>
--EXPECTF--
Done
Hello World!
