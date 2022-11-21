--TEST--
Inc/dec various strings
--FILE--
<?php

echo "Using increment:\n";
$values = [
    '',
    ' ',
    // Numeric-ish values
    '0',
    '15.5',
    '1e10',
    // Alphanumeric values
    '199A',
    'A199',
    '199Z',
    'Z199',
    // Strings
    'Hello world',
    '🐘'
];
foreach ($values as $value) {
    echo "Initial value:";
    var_dump($value);
    $value++;
    echo "Result value:";
    var_dump($value);
}

echo "Using decrement:\n";
$values = [
    '',
    ' ',
    // Numeric-ish values
    '0',
    '15.5',
    '1e10',
    // Alphanumeric values
    '199A',
    'A199',
    '199Z',
    'Z199',
    // Strings
    'Hello world',
    '🐘'
];
foreach ($values as $value) {
    echo "Initial value:";
    var_dump($value);
    $value--;
    echo "Result value:";
    var_dump($value);
}
?>
--EXPECT--
Using increment:
Initial value:string(0) ""
Result value:string(1) "1"
Initial value:string(1) " "
Result value:string(1) " "
Initial value:string(1) "0"
Result value:int(1)
Initial value:string(4) "15.5"
Result value:float(16.5)
Initial value:string(4) "1e10"
Result value:float(10000000001)
Initial value:string(4) "199A"
Result value:string(4) "199B"
Initial value:string(4) "A199"
Result value:string(4) "A200"
Initial value:string(4) "199Z"
Result value:string(4) "200A"
Initial value:string(4) "Z199"
Result value:string(4) "Z200"
Initial value:string(11) "Hello world"
Result value:string(11) "Hello worle"
Initial value:string(4) "🐘"
Result value:string(4) "🐘"
Using decrement:
Initial value:string(0) ""
Result value:int(-1)
Initial value:string(1) " "
Result value:string(1) " "
Initial value:string(1) "0"
Result value:int(-1)
Initial value:string(4) "15.5"
Result value:float(14.5)
Initial value:string(4) "1e10"
Result value:float(9999999999)
Initial value:string(4) "199A"
Result value:string(4) "199A"
Initial value:string(4) "A199"
Result value:string(4) "A199"
Initial value:string(4) "199Z"
Result value:string(4) "199Z"
Initial value:string(4) "Z199"
Result value:string(4) "Z199"
Initial value:string(11) "Hello world"
Result value:string(11) "Hello world"
Initial value:string(4) "🐘"
Result value:string(4) "🐘"
