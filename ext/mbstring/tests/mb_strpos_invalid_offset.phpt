--TEST--
mb_strpos() with invalid offsets
--EXTENSIONS--
mbstring
--FILE--
<?php

ini_set('include_path','.');
include_once('common.inc');
mb_internal_encoding('UTF-8') or print("mb_internal_encoding() failed\n");

// Test string
$string = '0123この文字列は日本語です。UTF-8を使っています。0123日本語は面倒臭い。';

$slen = mb_strlen($string);
echo "String len: $slen\n";

print ("== INVALID OFFSET ==\n");

try {
    var_dump( mb_strpos($string, '日本語', 44));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump( mb_strpos($string, '日本語', 50));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump( mb_strpos($string, '0', 50));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_strpos($string, 3, 50));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_strpos($string, 0, 50));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_strpos($string, '日本語', -50));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_strpos($string, '0', -50));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_strpos($string, 3, -50));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_strpos($string, 0, -50));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_strpos($string, 0, -44));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
String len: 42
== INVALID OFFSET ==
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
