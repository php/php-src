--TEST--
Bug #71221 (Null pointer deref (segfault) in get_defined_vars via ob_start)
--FILE--
<?php
try {
    ob_start("get_defined_vars");
} catch (\Error $e) {
    echo $e->getMessage();
}
ob_end_clean();
?>

OKAY
--EXPECT--
Cannot call get_defined_vars() dynamically
OKAY
