--TEST--
Test extract() function - error condition - Invalid prefix.
--FILE--
<?php
$a = ["1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five"];

try {
    extract($a, EXTR_PREFIX_ALL, '85bogus');
} catch (\ValueError $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
extract(): Argument #3 ($prefix) must be a valid identifier
