--TEST--
Bug #71220 (Null pointer deref (segfault) in compact via ob_start)
--FILE--
<?php
ob_start("compact");
try {
    ob_end_clean();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot call compact() dynamically
