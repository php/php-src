--TEST--
exit() and finally (1)
--FILE--
<?php

// TODO: In the future, we should execute the finally block.

try {
    exit("Exit\n");
} catch (Throwable $e) {
    echo "Not caught\n";
} finally {
    echo "Finally\n";
}
echo "Not executed\n";

?>
--EXPECT--
Exit
