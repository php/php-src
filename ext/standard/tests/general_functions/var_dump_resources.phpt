--TEST--
Test var_dump() function with resources
--FILE--
<?php

$values = [
    STDIN,
    STDERR,
];

foreach ($values as $value) {
    $ret_string = var_dump($value);
}

?>
--EXPECT--
resource(1) of type (stream)
resource(3) of type (stream)
