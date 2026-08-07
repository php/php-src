--TEST--
Bug #48428 (crash when exception is thrown while passing function arguments)
--FILE--
<?php
try {
        function x() { throw new Exception("ERROR"); }
                x(x());
} catch(Exception $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Exception: ERROR
