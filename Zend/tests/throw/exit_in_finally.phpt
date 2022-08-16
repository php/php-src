--TEST--
Test exit with uncaught exception in finally block
--FILE--
<?php
try {
    throw new Exception();
} finally {
    echo "Done\n";
    exit();
}
?>
--EXPECT--
Done
