--TEST--
'function_and_const_lookup=global' with multiple namespaces
--INI--
error_reporting=E_ALL
--FILE--
<?php
declare(function_and_const_lookup=global);
namespace Other {
use Error;

const OTHER_TEST_CONSTANT = 'Other test constant';
try {
    echo "OTHER_TEST_CONSTANT=" . OTHER_TEST_CONSTANT . "\n";
} catch (Error $e) {
    echo "Caught: {$e->getMessage()}\n";
}
use const Other\OTHER_TEST_CONSTANT;
echo "namespace\OTHER_TEST_CONSTANT=" . namespace\OTHER_TEST_CONSTANT . "\n";

}

namespace Test {
function printf(string $msg, ...$args) {
    \printf("Prefix: $msg", ...$args);
}
const TEST_CONSTANT = 'Namespaced constant';

// 'use function/const *;' will make PHP skip the check for functions/constants in the same namespace,
// unless there is an alias.
// This happens even if the file defines a function/constant of the same name.
use function Other\printf as other_printf;
use const Other\OTHER_TEST_CONSTANT;
use const Other\OTHER_TEST_CONSTANT as ALIASED_TEST_CONSTANT;

define('TEST_CONSTANT', 'Global constant');

printf("Test %d\n", 1);
namespace\printf("Test %d\n", 2);
\printf("Test %d\n", 3);
other_printf("Aliases should continue to work\n");

echo "Not fully qualified: " . TEST_CONSTANT . "\n";
echo "Fully qualified: " . \TEST_CONSTANT . "\n";
echo "Namespace relative: " . namespace\TEST_CONSTANT . "\n";
echo "Used constant: " . OTHER_TEST_CONSTANT . "\n";
echo "Aliased constant: " . ALIASED_TEST_CONSTANT . "\n";

}

namespace {
echo "\nIn the global namespace\n";
printf("Test %d\n", 1);
namespace\printf("Test %d\n", 2);
\printf("Test %d\n", 3);
echo "Not fully qualified: " . TEST_CONSTANT . "\n";
echo "Fully qualified: " . \TEST_CONSTANT . "\n";
echo "Namespace relative: " . namespace\TEST_CONSTANT . "\n";
}

namespace Other {
function printf(string $msg, ...$args) {
    echo "Stub for printf: $msg";
}
}
--EXPECT--
Caught: Undefined constant 'OTHER_TEST_CONSTANT'
namespace\OTHER_TEST_CONSTANT=Other test constant
Test 1
Prefix: Test 2
Test 3
Stub for printf: Aliases should continue to work
Not fully qualified: Global constant
Fully qualified: Global constant
Namespace relative: Namespaced constant
Used constant: Other test constant
Aliased constant: Other test constant

In the global namespace
Test 1
Test 2
Test 3
Not fully qualified: Global constant
Fully qualified: Global constant
Namespace relative: Global constant