--TEST--
Make sure is_callable error does not leak if an exception is also thrown
--FILE--
<?php
spl_autoload_register(function ($class) {
    throw new Exception("Failed");
});
try {
    array_map('A::b', []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Failed
