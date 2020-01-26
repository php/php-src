--TEST--
function_and_const_lookup="global" with redundant uses.
--FILE--
<?php
declare(function_and_const_lookup="global");
namespace Other {
const MY_CONSTANT = 'MY_CONSTANT(NAMESPACED)';
}
namespace {
const MY_CONSTANT = 'MY_CONSTANT(GLOBAL)';
}

namespace Other {
use const MY_CONSTANT;
use function printf as printf_original;
echo "MY_CONSTANT (not FQ) = " . MY_CONSTANT . "\n";
echo "MY_CONSTANT (namespace relative) = " . namespace\MY_CONSTANT . "\n";

function printf(string $message, ...$args) {
    echo "Stub for printf $message";
}
printf("Test\n");  // this is global, because there's function_and_const_lookup="global" and no 'use function Other\printf'
printf_original("Test printf_original\n");
\printf("Test\n");
namespace\printf("Test\n");
}

namespace {
use const MY_CONSTANT;  // should warn
use const MISSING as MISSING;  // should warn
use const OTHER_MISSING as other_missing;  // should not warn
echo "MY_CONSTANT = " . MY_CONSTANT . "\n";
}
--EXPECTF--
Warning: The 'use const MY_CONSTANT;' is redundant due to 'declare(function_and_const_lookup="global")' in %s on line 11

Warning: The use statement with non-compound name 'MY_CONSTANT' has no effect in %s on line 26

Warning: The 'use const MY_CONSTANT;' is redundant due to 'declare(function_and_const_lookup="global")' in %s on line 26

Warning: The 'use const MISSING;' is redundant due to 'declare(function_and_const_lookup="global")' in %s on line 27
MY_CONSTANT (not FQ) = MY_CONSTANT(GLOBAL)
MY_CONSTANT (namespace relative) = MY_CONSTANT(NAMESPACED)
Test
Test printf_original
Test
Stub for printf Test
MY_CONSTANT = MY_CONSTANT(GLOBAL)
