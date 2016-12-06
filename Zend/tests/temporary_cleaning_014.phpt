--TEST--
Leak in JMP_SET
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
