--TEST--
Test array_any() function : basic functionality
--FILE--
<?php
$array1 = [
    "a" => 1,
    "b" => 2,
    "c" => 3,
    "d" => 4,
    "e" => 5,
];

$array2 = [
    1, 2, 3, 4, 5
];

function even($input) {
  return $input % 2 === 0;
}

class EvenClass {
    public static function even($input) {
        return $input % 2 === 0;
    }
}


var_dump(array_any($array1, fn($value) => $value > 3));
var_dump(array_any($array2, fn($value) => $value > 3));
var_dump(array_any($array2, fn($value) => $value > 5));
var_dump(array_any([], fn($value) => true));
var_dump(array_any($array1, fn($value, $key) => $key === "c"));
var_dump(array_any($array1, fn($value, $key) => false));

echo '*** Test Exception after true result ***' . PHP_EOL;
try {
    var_dump(array_any($array2, function ($value) {
        if ($value > 1) {
            throw new Exception("Test-Exception");
        }

        return true;
    }));
} catch (Exception) {
    var_dump("Unexpected Exception");
}

echo '*** Test aborting with exception ***' . PHP_EOL;
try {
    var_dump(array_any($array2, function ($value) {
        if ($value === 2) {
            throw new Exception("Test-Exception");
        }

        var_dump($value);

        return false;
    }));
} catch (Exception) {
    var_dump("Catched Exception");
}

var_dump(array_any($array1, 'even'));

var_dump(array_any($array1, function($value) {
    // return nothing
}));

var_dump(array_any($array1, [
    'EvenClass',
    'even'
]));

var_dump(array_any($array1, "EvenClass::even"));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
*** Test Exception after true result ***
bool(true)
*** Test aborting with exception ***
int(1)
string(17) "Catched Exception"
bool(true)
bool(false)
bool(true)
bool(true)
