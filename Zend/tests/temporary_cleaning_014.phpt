--TEST--
Leak in JMP_SET
--EXTENSIONS--
gmp
--FILE--
<?php
set_error_handler(function() { throw new Exception; });
try {
    gmp_init(0) ?: null;
} catch (Exception $e) {
}
?>
DONE
--EXPECT--
DONE
