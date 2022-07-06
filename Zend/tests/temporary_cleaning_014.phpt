--TEST--
Leak in JMP_SET
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
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
