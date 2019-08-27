--TEST--
Test extract() function - error condition - Invalid flag argument.
--FILE--
<?php
$a = ["1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five"];

try {
    extract($a, 10000);
} catch (\Error $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Invalid extract type
