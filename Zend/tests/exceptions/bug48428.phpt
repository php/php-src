--TEST--
Bug #48428 (crash when exception is thrown while passing function arguments)
--FILE--
<?php
try {
        function x() { throw new Exception("ERROR"); }
                x(x());
} catch(Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: ERROR
