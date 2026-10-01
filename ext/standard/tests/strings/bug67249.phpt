--TEST--
Bug #67249 (printf out-of-bounds read)
--FILE--
<?php
try {
    var_dump(sprintf("%'", "foo"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Missing padding character
