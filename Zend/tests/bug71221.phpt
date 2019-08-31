--TEST--
Bug #71221 (Null pointer deref (segfault) in get_defined_vars via ob_start)
--FILE--
<?php
ob_start("get_defined_vars");
try {
    ob_end_clean();
} catch (\Error $e) {
    echo $e->getMessage();
}
?>

OKAY
--EXPECT--
Cannot call get_defined_vars() dynamically
OKAY
