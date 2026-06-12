--TEST--
__exists: property_exists() is not affected by __exists (mirrors __isset behaviour)
--FILE--
<?php

/* property_exists() is a reflection-style operator over the declared class
 * shape plus instance dynamic properties. It must not consult __exists,
 * exactly as it does not consult __isset today. Users who want
 * "magic-aware existence" call $obj->__exists($name) directly. */

class C {
    public int $declared = 1;
    public function __exists(string $n): bool {
        echo "  __exists($n) MUST NOT be called\n";
        return true;
    }
    public function __get(string $n): mixed { return null; }
}

$c = new C;

echo "1) property_exists() on a declared property: true (no magic call):\n";
var_dump(property_exists($c, 'declared'));

echo "\n2) property_exists() on a non-existent property: false (no magic call):\n";
var_dump(property_exists($c, 'missing'));

echo "\n3) property_exists(class-string, ...) accepts class shape only:\n";
var_dump(property_exists('C', 'declared'));
var_dump(property_exists('C', 'missing'));

?>
--EXPECT--
1) property_exists() on a declared property: true (no magic call):
bool(true)

2) property_exists() on a non-existent property: false (no magic call):
bool(false)

3) property_exists(class-string, ...) accepts class shape only:
bool(true)
bool(false)
