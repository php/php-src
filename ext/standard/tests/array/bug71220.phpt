--TEST--
Bug #71220 (Null pointer deref (segfault) in compact via ob_start)
--FILE--
<?php
ob_start("compact");
try {
    ob_end_clean();
} catch (\Error $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Cannot call compact() dynamically
