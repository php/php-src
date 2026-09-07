--TEST--
Testing nested exceptions
--FILE--
<?php

try {
    try {
        try {
            try {
                throw new Exception();
            } catch (Throwable $e) {
                echo $e::class, ': "', $e->getMessage(), '"', "\n";
                throw $e;
            }
        } catch (Throwable $e) {
            echo $e::class, ': "', $e->getMessage(), '"', "\n";
            throw $e;
        }
    } catch (Throwable $e) {
        echo $e::class, ': "', $e->getMessage(), '"', "\n";
        throw $e;
    }
} catch (Throwable $e) {
    echo $e::class, ': "', $e->getMessage(), '"', "\n";
    throw $e;
}

?>
--EXPECTF--
Exception: ""
Exception: ""
Exception: ""
Exception: ""

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
