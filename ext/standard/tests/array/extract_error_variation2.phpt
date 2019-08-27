--TEST--
Test extract() function - error condition - Prefix flag without supplying prefix.
--FILE--
<?php
$a = ["1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five"];

try {
    extract($a, EXTR_PREFIX_ALL);
} catch (\Error $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Specified extract type requires the prefix parameter
