--TEST--
Bug #67249 (printf out-of-bounds read)
--FILE--
<?php
try {
    var_dump(sprintf("%'", "foo"));
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Missing padding character
