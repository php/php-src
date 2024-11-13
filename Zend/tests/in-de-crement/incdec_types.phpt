--TEST--
Inc/dec various types
--FILE--
<?php

/* Type errors */
$types = [[], new stdClass(), fopen(__FILE__, 'r')];

foreach ($types as $type) {
    try {
        $type++;
    } catch (\TypeError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
    try {
        $type--;
    } catch (\TypeError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

echo "Using increment:\n";
$values = [null, false, true, 0, 0.0, '', ' ', '0'];
foreach ($values as $value) {
    echo "Initial value:";
    var_dump($value);
    $value++;
    echo "Result value:";
    var_dump($value);
}

echo "Using decrement:\n";
$values = [null, false, true, 0, 0.0, '', ' ', '0'];
foreach ($values as $value) {
    echo "Initial value:";
    var_dump($value);
    $value--;
    echo "Result value:";
    var_dump($value);
}
?>
--EXPECTF--
Cannot increment array
Cannot decrement array
Cannot increment stdClass
Cannot decrement stdClass
Cannot increment resource
Cannot decrement resource
Using increment:
Initial value:NULL
Result value:int(1)
Initial value:bool(false)

Warning: Increment on type bool has no effect, this will change in the next major version of PHP in %s on line %d
Result value:bool(false)
Initial value:bool(true)

Warning: Increment on type bool has no effect, this will change in the next major version of PHP in %s on line %d
Result value:bool(true)
Initial value:int(0)
Result value:int(1)
Initial value:float(0)
Result value:float(1)
Initial value:string(0) ""

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
Result value:string(1) "1"
Initial value:string(1) " "

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
Result value:string(1) " "
Initial value:string(1) "0"
Result value:int(1)
Using decrement:
Initial value:NULL

Warning: Decrement on type null has no effect, this will change in the next major version of PHP in %s on line %d
Result value:NULL
Initial value:bool(false)

Warning: Decrement on type bool has no effect, this will change in the next major version of PHP in %s on line %d
Result value:bool(false)
Initial value:bool(true)

Warning: Decrement on type bool has no effect, this will change in the next major version of PHP in %s on line %d
Result value:bool(true)
Initial value:int(0)
Result value:int(-1)
Initial value:float(0)
Result value:float(-1)
Initial value:string(0) ""

Deprecated: Decrement on empty string is deprecated as non-numeric in %s on line %d
Result value:int(-1)
Initial value:string(1) " "

Deprecated: Decrement on non-numeric string has no effect and is deprecated in %s on line %d
Result value:string(1) " "
Initial value:string(1) "0"
Result value:int(-1)
