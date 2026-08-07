--TEST--
Levenshtein suggestion for undefined function in FCC context
--FILE--
<?php
// Test 1: In default argument position

function test(Closure $name = array_pussh(...)) {
    var_dump($name);
}

try {
    test();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Test 2: In constant initializer (via eval so the fatal can be caught for combined output)

try {
    eval("const Closure2 = strlenn(...);");
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Call to undefined function array_pussh() (did you mean array_push?)
Error: Call to undefined function strlenn() (did you mean strlen?)
