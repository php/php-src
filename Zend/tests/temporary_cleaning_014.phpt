--TEST--
Leak in JMP_SET
--EXTENSIONS--
gmp
--FILE--
<?php
set_error_handler(function() { throw new Exception; });
try {
    new GMP ?: null;
} catch (Exception $e) {
}
?>
DONE
--EXPECT--
DONE
