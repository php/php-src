--TEST--
Test array_all() function : basic functionality
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

class SmallerTenClass {
    public static function smallerTen($input) {
        return $input < 10;
    }
}

var_dump(array_all($array1, fn($value) => $value > 0));
var_dump(array_all($array2, fn($value) => $value > 0));
var_dump(array_all($array2, fn($value) => $value > 1));
var_dump(array_all([], fn($value) => true));

echo '*** Test Exception after false result ***' . PHP_EOL;
try {
    var_dump(array_all($array2, function ($value) {
        if ($value > 1) {
            throw new Exception("Test-Exception");
        }

        return false;
    }));
} catch (Exception) {
    var_dump("Unexpected Exception");
}

echo '*** Test aborting with exception ***' . PHP_EOL;
try {
    var_dump(array_all($array2, function ($value) {
        if ($value === 2) {
            throw new Exception("Test-Exception");
        }

        var_dump($value);

        return true;
    }));
} catch (Exception) {
    var_dump("Catched Exception");
}

var_dump(array_all($array1, 'even'));

var_dump(array_all($array1, function($value) {
    // return nothing
}));

var_dump(array_all($array1, [
    'SmallerTenClass',
    'smallerTen'
]));

var_dump(array_all($array1, "SmallerTenClass::smallerTen"));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
*** Test Exception after false result ***
bool(false)
*** Test aborting with exception ***
int(1)
string(17) "Catched Exception"
bool(false)
bool(false)
bool(true)
bool(true)
