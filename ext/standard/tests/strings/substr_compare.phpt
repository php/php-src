--TEST--
substr_compare()
--FILE--
<?php

var_dump(substr_compare("abcde", "df", -2) < 0);
var_dump(substr_compare("abcde", "df", -2, null) < 0);
var_dump(substr_compare("abcde", "bc", 1, 2));
var_dump(substr_compare("abcde", "bcg", 1, 2));
var_dump(substr_compare("abcde", "BC", 1, 2, true));
var_dump(substr_compare("abcde", "bc", 1, 3) > 0);
var_dump(substr_compare("abcde", "cd", 1, 2) < 0);
var_dump(substr_compare("abcde", "abc", 5, 1));
var_dump(substr_compare("abcde", "abcdef", -10, 10) < 0);
var_dump(substr_compare("abcde", "abc", 0, 0));
echo "Test\n";

try {
    substr_compare("abcde", "abc", 0, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
var_dump(substr_compare("abcde", "abc", -1, NULL, -5) > 0);
?>
--EXPECT--
bool(true)
bool(true)
int(0)
int(0)
int(0)
bool(true)
bool(true)
int(-1)
bool(true)
int(0)
Test
substr_compare(): Argument #4 ($length) must be greater than or equal to 0
bool(true)
