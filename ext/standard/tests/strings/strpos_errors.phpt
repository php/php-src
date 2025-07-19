--TEST--
strpos() error conditions
--INI--
precision=14
--FILE--
<?php

$string = <<<EOD
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
EOD;

echo "\n*** Testing error conditions ***\n";
var_dump(strpos($string, ""));
try {
    // offset > strlen()
    strpos($string, "test", strlen($string)+1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    // offset before start
    strpos($string, "test", -strlen($string)-1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing error conditions ***
int(0)
strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
