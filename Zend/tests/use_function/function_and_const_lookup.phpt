--TEST--
'function_and_const_lookup='global'' with multiple namespaces
--INI--
error_reporting=E_ALL
--FILE--
<?php
declare(function_and_const_lookup='global');
namespace Other {
use Error;
use const Other\OTHER_TEST_CONSTANT;

const OTHER_TEST_CONSTANT = 'Other test constant';
echo "OTHER_TEST_CONSTANT=" . OTHER_TEST_CONSTANT . "\n";
echo "namespace\OTHER_TEST_CONSTANT=" . namespace\OTHER_TEST_CONSTANT . "\n";
try {
    echo "\OTHER_TEST_CONSTANT=" . \OTHER_TEST_CONSTANT . "\n";
} catch (Error $e) {
    echo "Caught {$e->getMessage()}\n";
}
}

namespace Test {
use function Test\printf;
use const Test\TEST_CONSTANT;
function printf(string $msg, ...$args) {
    // The requirement that 'use function Test\printf;' is explicitly declared in an above statement
    // forces confusing code like this to be fully qualified.
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
function my_function() {
    echo "In " . __FUNCTION__ . "\n";
}
my_function();
}

namespace Other {
use function Other\printf;
function printf(string $msg, ...$args) {
    echo "Stub for printf: $msg";
}
}
--EXPECT--
OTHER_TEST_CONSTANT=Other test constant
namespace\OTHER_TEST_CONSTANT=Other test constant
Caught Undefined constant 'OTHER_TEST_CONSTANT'
Prefix: Test 1
Prefix: Test 2
Test 3
Stub for printf: Aliases should continue to work
Not fully qualified: Namespaced constant
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
In my_function