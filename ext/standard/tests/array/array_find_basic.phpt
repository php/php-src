--TEST--
Test array_find() function : basic functionality
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

var_dump(array_find($array1, fn($value) => $value > 3));
var_dump(array_find($array2, fn($value) => $value > 3));
var_dump(array_find($array2, fn($value) => $value > 5));
var_dump(array_find([], fn($value) => true));
var_dump(array_find($array1, fn($value, $key) => $key === "c"));
var_dump(array_find($array1, fn($value, $key) => false));

echo '*** Test Exception after found result ***' . PHP_EOL;
try {
    var_dump(array_find($array1, function ($value) {
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
    var_dump(array_find($array2, function ($value) {
        if ($value === 2) {
            throw new Exception("Test-Exception");
        }

        var_dump($value);

        return false;
    }));
} catch (Exception) {
    var_dump("Catched Exception");
}

var_dump(array_find($array1, 'even'));

var_dump(array_find($array1, function($value) {
    // return nothing
}));

var_dump(array_find($array1, [
    'EvenClass',
    'even'
]));

var_dump(array_find($array1, "EvenClass::even"));
?>
--EXPECT--
int(4)
int(4)
NULL
NULL
int(3)
NULL
*** Test Exception after found result ***
int(1)
*** Test aborting with exception ***
int(1)
string(17) "Catched Exception"
int(2)
NULL
int(2)
int(2)
