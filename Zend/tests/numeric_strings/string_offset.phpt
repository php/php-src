--TEST--
Using different sorts of numerical strings as a string offset
--FILE--
<?php

$str = "The world is fun";

$keys = [
    "7",
    "7.5",
    "  7",
    "  7.5",
    "  7  ",
    "  7.5  ",
    "7  ",
    "7.5  ",
    "7str",
    "7.5str",
    "  7str",
    "  7.5str",
    "  7  str",
    "  7.5  str",
    "7  str",
    "7.5  str",
    "0xC",
    "0b10",
    "07",
];

foreach ($keys as $key) {
    try {
        var_dump($str[$key]);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

echo "Done\n";
?>
--EXPECTF--
string(1) "l"
Cannot access offset of type string on string
string(1) "l"
Cannot access offset of type string on string
string(1) "l"
Cannot access offset of type string on string
string(1) "l"
Cannot access offset of type string on string

Warning: Illegal string offset "7str" in %s on line %d
string(1) "l"
Cannot access offset of type string on string

Warning: Illegal string offset "  7str" in %s on line %d
string(1) "l"
Cannot access offset of type string on string

Warning: Illegal string offset "  7  str" in %s on line %d
string(1) "l"
Cannot access offset of type string on string

Warning: Illegal string offset "7  str" in %s on line %d
string(1) "l"
Cannot access offset of type string on string

Warning: Illegal string offset "0xC" in %s on line %d
string(1) "T"

Warning: Illegal string offset "0b10" in %s on line %d
string(1) "T"
string(1) "l"
Done
