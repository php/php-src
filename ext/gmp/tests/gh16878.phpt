--TEST--
GH-16878: Core dump when gmp_fact allocates huge memory
--EXTENSIONS--
gmp
--FILE--
<?php
echo "Test 1: Factorial of 2^50 + 1\n";
try {
    $value = gmp_add(gmp_pow(2, 50), 1);
    echo "Calculating factorial of: ", gmp_strval($value), "\n";
    $result = gmp_fact($value);
    echo "Result: " . gmp_strval($result) . "\n";
} catch (\ValueError $e) {
    echo "ValueError: " . $e->getMessage() . "\n";
} catch (\Error $e) {
    echo "Error: " . $e->getMessage() . "\n";
}

echo "\nTest 2: Another large value\n";
try {
    $value = gmp_init('1000000000000');  // 1 trillion
    echo "Calculating factorial of: ", gmp_strval($value), "\n";
    $result = gmp_fact($value);
    echo "Result: " . gmp_strval($result) . "\n";
} catch (\ValueError $e) {
    echo "ValueError: " . $e->getMessage() . "\n";
} catch (\Error $e) {
    echo "Error: " . $e->getMessage() . "\n";
}

echo "\nTest 3: Moderately large value that should work\n";
try {
    $value = 100;
    echo "Calculating factorial of: $value\n";
    $result = gmp_fact($value);
    echo "Result length: " . strlen(gmp_strval($result)) . " digits\n";
} catch (\ValueError $e) {
    echo "ValueError: " . $e->getMessage() . "\n";
} catch (\Error $e) {
    echo "Error: " . $e->getMessage() . "\n";
}

echo "\nDone\n";
?>
--EXPECTF--
Test 1: Factorial of 2^50 + 1
Calculating factorial of: 1125899906842625
ValueError: %s

Test 2: Another large value
Calculating factorial of: 1000000000000
ValueError: %s

Test 3: Moderately large value that should work
Calculating factorial of: 100
Result length: 158 digits

Done