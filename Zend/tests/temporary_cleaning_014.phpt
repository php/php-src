--TEST--
Leak in JMP_SET
--EXTENSIONS--
gmp
--FILE--
<?php
set_exception_handler(function() { throw new Exception; });
try {
    new GMP ?: null;
} catch (\TypeError $e) {
}
?>
DONE
--EXPECT--
DONE
