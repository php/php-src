--TEST--
exit() and finally (2)
--FILE--
<?php

try {
    try {
        exit("Exit\n");
    } catch (Throwable $e) {
        echo "Not caught\n";
    } finally {
        throw new Exception("Finally exception");
    }
    echo "Not executed\n";
} catch (Exception $e) {
    echo "Caught {$e->getMessage()}\n";
}

?>
--EXPECT--
Caught Finally exception
