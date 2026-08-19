--TEST--
Test extract() function - error condition - Invalid prefix.
--FILE--
<?php
$a = ["1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five"];

try {
    extract($a, EXTR_PREFIX_ALL, '85bogus');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: extract(): Argument #3 ($prefix) must be a valid identifier
