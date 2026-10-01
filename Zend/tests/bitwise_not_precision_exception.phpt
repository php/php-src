--TEST--
Promoting float precision warning to exception in bitwise_not
--FILE--
<?php
set_error_handler(function($_, $msg) {
    throw new Exception($msg);
});
try {
    var_dump(~INF);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: The float INF is not representable as an int, cast occurred
