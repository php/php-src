--TEST--
Test substr_count() function (basic)
--FILE--
<?php

echo "***Testing basic operations ***\n";
try {
    substr_count("", "");
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    substr_count("a", "");
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
var_dump(substr_count("", "a"));
var_dump(substr_count("", "a"));
var_dump(substr_count("", chr(0)));

$a = str_repeat("abcacba", 100);
var_dump(substr_count($a, "bca"));

$a = str_repeat("abcacbabca", 100);
var_dump(substr_count($a, "bca"));
var_dump(substr_count($a, "bca", 200));
var_dump(substr_count($a, "bca", 200, null));
var_dump(substr_count($a, "bca", 200, 50));
var_dump(substr_count($a, "bca", -200));
var_dump(substr_count($a, "bca", -200, null));
var_dump(substr_count($a, "bca", -200, 50));
var_dump(substr_count($a, "bca", -200, -50));

?>
--EXPECT--
***Testing basic operations ***
substr_count(): Argument #2 ($needle) cannot be empty
substr_count(): Argument #2 ($needle) cannot be empty
int(0)
int(0)
int(0)
int(100)
int(200)
int(160)
int(160)
int(10)
int(40)
int(40)
int(10)
int(30)
