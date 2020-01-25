--TEST--
Cannot use backtick shorthand for shell_exec in constant expressions
--FILE--
<?php
// Make sure that this is not accidentally allowed by future changes to PHP.
// During compilation, this gets converted to a regular function call opcode.
class Example {
    const TEST_CONST = [`echo test`];
}
?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s on line 5
