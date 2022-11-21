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
--EXPECT--
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
Result value:bool(false)
Initial value:bool(true)
Result value:bool(true)
Initial value:int(0)
Result value:int(1)
Initial value:float(0)
Result value:float(1)
Initial value:string(0) ""
Result value:string(1) "1"
Initial value:string(1) " "
Result value:string(1) " "
Initial value:string(1) "0"
Result value:int(1)
Using decrement:
Initial value:NULL
Result value:NULL
Initial value:bool(false)
Result value:bool(false)
Initial value:bool(true)
Result value:bool(true)
Initial value:int(0)
Result value:int(-1)
Initial value:float(0)
Result value:float(-1)
Initial value:string(0) ""
Result value:int(-1)
Initial value:string(1) " "
Result value:string(1) " "
Initial value:string(1) "0"
Result value:int(-1)
