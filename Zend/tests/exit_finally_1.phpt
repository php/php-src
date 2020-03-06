--TEST--
exit() and finally (1)
--FILE--
<?php

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
Finally
Exit
