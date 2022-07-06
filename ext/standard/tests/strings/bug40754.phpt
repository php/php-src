--TEST--
Bug #40754 (Overflow checks inside string functions)
--FILE--
<?php

$v = 2147483647;

var_dump(substr("abcde", 1, $v));
var_dump(substr_replace("abcde", "x", $v, $v));
var_dump(strspn("abcde", "abc", $v, $v));
var_dump(strcspn("abcde", "abc", $v, $v));

try {
    var_dump(substr_count("abcde", "abc", $v, $v));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    substr_compare("abcde", "abc", $v, $v);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    stripos("abcde", "abc", $v);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    substr_count("abcde", "abc", $v, 1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    substr_count("abcde", "abc", 1, $v);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strpos("abcde", "abc", $v);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    stripos("abcde", "abc", $v);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strrpos("abcde", "abc", $v);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strripos("abcde", "abc", $v);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strripos("abcde", "abc", $v);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(strncmp("abcde", "abc", $v));
var_dump(chunk_split("abcde", $v, "abc"));
var_dump(substr("abcde", $v, $v));

?>
--EXPECT--
string(4) "bcde"
string(6) "abcdex"
int(0)
int(0)
substr_count(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
substr_compare(): Argument #3 ($offset) must be contained in argument #1 ($main_str)
stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
substr_count(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
substr_count(): Argument #4 ($length) must be contained in argument #1 ($haystack)
strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
int(2)
string(8) "abcdeabc"
string(0) ""
