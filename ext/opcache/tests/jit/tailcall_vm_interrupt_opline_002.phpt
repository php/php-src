--TEST--
Function JIT: preserve the next opline when handling a VM interrupt
--DESCRIPTION--
The object comparison raises an interrupt while the minimal function JIT
executes a callable opcode handler. The interrupt helper must resume at the
branch target, not reload the saved comparison opcode. No interrupt callback
is installed, isolating the saved-opline bug from callback resume-tag handling.
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=1201
opcache.jit_buffer_size=64M
--FILE--
<?php
function compare($left, $right) {
    if ($left == $right) {
        return 11;
    }
    return 22;
}

var_dump(compare(new VmInterruptComparable(1), new VmInterruptComparable(2)));
?>
--EXPECT--
int(22)
