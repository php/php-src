--TEST--
Test print_r() function with resources
--FILE--
<?php

$values = [
    STDIN,
    STDERR,
];

foreach ($values as $value) {
    print_r($value, false);
    // $ret_string captures the output
    $ret_string = print_r($value, true);
    echo "\n$ret_string\n";
}

?>
--EXPECT--
Resource id #1
Resource id #1
Resource id #3
Resource id #3
