--TEST--
strict_types settings are respected
--FILE--
<?php
declare(strict_types=1);
// zend_call_function currently always adds a fake frame with strict_types,
// because the current opcode is an possibly error handler.
// This needs to be fixed.
const X = "100";
const RESULT = intdiv(X, 2);
echo "Result is " . RESULT . "\n";
?>
--EXPECT--
TODO: Throw
