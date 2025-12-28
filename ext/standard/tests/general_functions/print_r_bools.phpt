--TEST--
Test print_r() function with booleans
--FILE--
<?php

$values = [
  'true' => true,
  'false' => false,
];

foreach ($values as $key => $value) {
    echo "$key:\n";
    print_r($value, false);
    // $ret_string captures the output
    $ret_string = print_r($value, true);
    echo "\n$ret_string\n";
}

?>
DONE
--EXPECT--
true:
1
1
false:


DONE
