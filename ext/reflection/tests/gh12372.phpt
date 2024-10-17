--TEST--
GH-12371 ("internal error" in ReflectionParameter::getDefaultValue on Variable-length argument list)
--FILE--
<?php

function f(...$args){
    echo "see? arguments are optional.\n";
}

function test(string $name) {
    $ref = new ReflectionFunction($name);
    var_dump($ref->getParameters()[0]->isVariadic());
    var_dump($ref->getParameters()[0]->getDefaultValue());
    var_dump($ref->getParameters()[0]->isDefaultValueAvailable());
    var_dump($ref->getParameters()[0]->isDefaultValueConstant());
    var_dump($ref->getParameters()[0]->getDefaultValueConstantName());
}

echo "--- Test with f() ---\n";
test("f");
echo "--- Test with array_merge() ---\n";
test("array_merge");

?>
--EXPECT--
--- Test with f() ---
bool(true)
array(0) {
}
bool(true)
bool(false)
NULL
--- Test with array_merge() ---
bool(true)
array(0) {
}
bool(true)
bool(false)
NULL
