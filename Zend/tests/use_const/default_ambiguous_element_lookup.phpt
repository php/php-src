--TEST--
'function_and_const_lookup=default'
--FILE--
<?php
// default has no effect on name resolution
declare(function_and_const_lookup=default);
// Preserve the existing fatal error for redeclaring a const that was used?
// TODO: Should this forbid declaring *any* global const if 'function_and_const_lookup=global' was part of the current scope?
namespace Other {
const MY_CONSTANT = 'MY_CONSTANT(NAMESPACED)';
}
namespace {
const MY_CONSTANT = 'MY_CONSTANT(GLOBAL)';
}

namespace Other {
use function printf as printf_original;
echo "MY_CONSTANT (FQ) = " . \MY_CONSTANT . "\n";
echo "MY_CONSTANT (not FQ) = " . MY_CONSTANT . "\n";
echo "MY_CONSTANT (namespace relative) = " . namespace\MY_CONSTANT . "\n";

function printf(string $message, ...$args) {
    echo "Stub for printf $message";
}
printf("Test not FQ\n");  // this is not global
printf_original("Test printf_original\n");
\printf("Test FQ\n");
namespace\printf("Test\n");
}
--EXPECTF--
MY_CONSTANT (FQ) = MY_CONSTANT(GLOBAL)
MY_CONSTANT (not FQ) = MY_CONSTANT(NAMESPACED)
MY_CONSTANT (namespace relative) = MY_CONSTANT(NAMESPACED)
Stub for printf Test not FQ
Test printf_original
Test FQ
Stub for printf Test
