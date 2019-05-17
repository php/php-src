--TEST--
Test array_search() function : usage variations - different needle values
--FILE--
<?php
/*
 * Prototype  : mixed array_search ( mixed $needle, array $haystack [, bool $strict] )
 * Description: Searches haystack for needle and returns the key if it is found in the array, FALSE otherwise
 * Source Code: ext/standard/array.c
*/

/* Test array_search() with different possible needle values */

echo "*** Testing array_search() with different needle values ***\n";
$arrays = array (
  array(0),
  array("a" => "A", 2 => "B", "C" => 3, 4 => 4, "one" => 1, "" => NULL, "b", "ab", "abcd"),
  array(4, array(1, 2 => 3), "one" => 1, "5" => 5 ),
  array(-1, -2, -3, -4, -2.989888, "-0.005" => "neg0.005", 2.0 => "float2", "-.9" => -.9),
  array(TRUE, FALSE),
  array("", array()),
  array("abcd\x00abcd\x00abcd"),
  array("abcd\tabcd\nabcd\rabcd\0abcdefghij")
);

$array_compare = array (
  4,
  "4",
  4.00,
  "b",
  "5",
  -2,
  -2.0,
  -2.98989,
  "-.9",
  "True",
  "",
  array(),
  NULL,
  "ab",
  "abcd",
  0.0,
  -0,
  "abcd\x00abcd\x00abcd"
);
/* loop to check if elements in $array_compare exist in $arrays
   using array_search() */
$counter = 1;
foreach($arrays as $array) {
  foreach($array_compare as $compare) {
    echo "-- Iteration $counter --\n";
    //strict option OFF
    var_dump(array_search($compare,$array));
    //strict option ON
    var_dump(array_search($compare,$array,TRUE));
    //strict option OFF
    var_dump(array_search($compare,$array,FALSE));
    $counter++;
 }
}

echo "Done\n";
?>
--EXPECT--
*** Testing array_search() with different needle values ***
-- Iteration 1 --
bool(false)
bool(false)
bool(false)
-- Iteration 2 --
bool(false)
bool(false)
bool(false)
-- Iteration 3 --
bool(false)
bool(false)
bool(false)
-- Iteration 4 --
int(0)
bool(false)
int(0)
-- Iteration 5 --
bool(false)
bool(false)
bool(false)
-- Iteration 6 --
bool(false)
bool(false)
bool(false)
-- Iteration 7 --
bool(false)
bool(false)
bool(false)
-- Iteration 8 --
bool(false)
bool(false)
bool(false)
-- Iteration 9 --
bool(false)
bool(false)
bool(false)
-- Iteration 10 --
int(0)
bool(false)
int(0)
-- Iteration 11 --
int(0)
bool(false)
int(0)
-- Iteration 12 --
bool(false)
bool(false)
bool(false)
-- Iteration 13 --
int(0)
bool(false)
int(0)
-- Iteration 14 --
int(0)
bool(false)
int(0)
-- Iteration 15 --
int(0)
bool(false)
int(0)
-- Iteration 16 --
int(0)
bool(false)
int(0)
-- Iteration 17 --
int(0)
int(0)
int(0)
-- Iteration 18 --
int(0)
bool(false)
int(0)
-- Iteration 19 --
int(4)
int(4)
int(4)
-- Iteration 20 --
int(4)
bool(false)
int(4)
-- Iteration 21 --
int(4)
bool(false)
int(4)
-- Iteration 22 --
int(5)
int(5)
int(5)
-- Iteration 23 --
bool(false)
bool(false)
bool(false)
-- Iteration 24 --
bool(false)
bool(false)
bool(false)
-- Iteration 25 --
bool(false)
bool(false)
bool(false)
-- Iteration 26 --
bool(false)
bool(false)
bool(false)
-- Iteration 27 --
bool(false)
bool(false)
bool(false)
-- Iteration 28 --
bool(false)
bool(false)
bool(false)
-- Iteration 29 --
string(0) ""
bool(false)
string(0) ""
-- Iteration 30 --
string(0) ""
bool(false)
string(0) ""
-- Iteration 31 --
string(0) ""
string(0) ""
string(0) ""
-- Iteration 32 --
int(6)
int(6)
int(6)
-- Iteration 33 --
int(7)
int(7)
int(7)
-- Iteration 34 --
string(1) "a"
bool(false)
string(1) "a"
-- Iteration 35 --
string(1) "a"
bool(false)
string(1) "a"
-- Iteration 36 --
bool(false)
bool(false)
bool(false)
-- Iteration 37 --
int(0)
int(0)
int(0)
-- Iteration 38 --
int(0)
bool(false)
int(0)
-- Iteration 39 --
int(0)
bool(false)
int(0)
-- Iteration 40 --
bool(false)
bool(false)
bool(false)
-- Iteration 41 --
int(5)
bool(false)
int(5)
-- Iteration 42 --
bool(false)
bool(false)
bool(false)
-- Iteration 43 --
bool(false)
bool(false)
bool(false)
-- Iteration 44 --
bool(false)
bool(false)
bool(false)
-- Iteration 45 --
bool(false)
bool(false)
bool(false)
-- Iteration 46 --
bool(false)
bool(false)
bool(false)
-- Iteration 47 --
bool(false)
bool(false)
bool(false)
-- Iteration 48 --
bool(false)
bool(false)
bool(false)
-- Iteration 49 --
bool(false)
bool(false)
bool(false)
-- Iteration 50 --
bool(false)
bool(false)
bool(false)
-- Iteration 51 --
bool(false)
bool(false)
bool(false)
-- Iteration 52 --
bool(false)
bool(false)
bool(false)
-- Iteration 53 --
bool(false)
bool(false)
bool(false)
-- Iteration 54 --
bool(false)
bool(false)
bool(false)
-- Iteration 55 --
bool(false)
bool(false)
bool(false)
-- Iteration 56 --
bool(false)
bool(false)
bool(false)
-- Iteration 57 --
bool(false)
bool(false)
bool(false)
-- Iteration 58 --
bool(false)
bool(false)
bool(false)
-- Iteration 59 --
bool(false)
bool(false)
bool(false)
-- Iteration 60 --
int(1)
int(1)
int(1)
-- Iteration 61 --
int(1)
bool(false)
int(1)
-- Iteration 62 --
bool(false)
bool(false)
bool(false)
-- Iteration 63 --
string(3) "-.9"
bool(false)
string(3) "-.9"
-- Iteration 64 --
bool(false)
bool(false)
bool(false)
-- Iteration 65 --
bool(false)
bool(false)
bool(false)
-- Iteration 66 --
bool(false)
bool(false)
bool(false)
-- Iteration 67 --
bool(false)
bool(false)
bool(false)
-- Iteration 68 --
bool(false)
bool(false)
bool(false)
-- Iteration 69 --
bool(false)
bool(false)
bool(false)
-- Iteration 70 --
int(2)
bool(false)
int(2)
-- Iteration 71 --
int(2)
bool(false)
int(2)
-- Iteration 72 --
bool(false)
bool(false)
bool(false)
-- Iteration 73 --
int(0)
bool(false)
int(0)
-- Iteration 74 --
int(0)
bool(false)
int(0)
-- Iteration 75 --
int(0)
bool(false)
int(0)
-- Iteration 76 --
int(0)
bool(false)
int(0)
-- Iteration 77 --
int(0)
bool(false)
int(0)
-- Iteration 78 --
int(0)
bool(false)
int(0)
-- Iteration 79 --
int(0)
bool(false)
int(0)
-- Iteration 80 --
int(0)
bool(false)
int(0)
-- Iteration 81 --
int(0)
bool(false)
int(0)
-- Iteration 82 --
int(0)
bool(false)
int(0)
-- Iteration 83 --
int(1)
bool(false)
int(1)
-- Iteration 84 --
int(1)
bool(false)
int(1)
-- Iteration 85 --
int(1)
bool(false)
int(1)
-- Iteration 86 --
int(0)
bool(false)
int(0)
-- Iteration 87 --
int(0)
bool(false)
int(0)
-- Iteration 88 --
int(1)
bool(false)
int(1)
-- Iteration 89 --
int(1)
bool(false)
int(1)
-- Iteration 90 --
int(0)
bool(false)
int(0)
-- Iteration 91 --
bool(false)
bool(false)
bool(false)
-- Iteration 92 --
bool(false)
bool(false)
bool(false)
-- Iteration 93 --
bool(false)
bool(false)
bool(false)
-- Iteration 94 --
bool(false)
bool(false)
bool(false)
-- Iteration 95 --
bool(false)
bool(false)
bool(false)
-- Iteration 96 --
bool(false)
bool(false)
bool(false)
-- Iteration 97 --
bool(false)
bool(false)
bool(false)
-- Iteration 98 --
bool(false)
bool(false)
bool(false)
-- Iteration 99 --
bool(false)
bool(false)
bool(false)
-- Iteration 100 --
bool(false)
bool(false)
bool(false)
-- Iteration 101 --
int(0)
int(0)
int(0)
-- Iteration 102 --
int(1)
int(1)
int(1)
-- Iteration 103 --
int(0)
bool(false)
int(0)
-- Iteration 104 --
bool(false)
bool(false)
bool(false)
-- Iteration 105 --
bool(false)
bool(false)
bool(false)
-- Iteration 106 --
int(0)
bool(false)
int(0)
-- Iteration 107 --
int(0)
bool(false)
int(0)
-- Iteration 108 --
bool(false)
bool(false)
bool(false)
-- Iteration 109 --
bool(false)
bool(false)
bool(false)
-- Iteration 110 --
bool(false)
bool(false)
bool(false)
-- Iteration 111 --
bool(false)
bool(false)
bool(false)
-- Iteration 112 --
bool(false)
bool(false)
bool(false)
-- Iteration 113 --
bool(false)
bool(false)
bool(false)
-- Iteration 114 --
bool(false)
bool(false)
bool(false)
-- Iteration 115 --
bool(false)
bool(false)
bool(false)
-- Iteration 116 --
bool(false)
bool(false)
bool(false)
-- Iteration 117 --
bool(false)
bool(false)
bool(false)
-- Iteration 118 --
bool(false)
bool(false)
bool(false)
-- Iteration 119 --
bool(false)
bool(false)
bool(false)
-- Iteration 120 --
bool(false)
bool(false)
bool(false)
-- Iteration 121 --
bool(false)
bool(false)
bool(false)
-- Iteration 122 --
bool(false)
bool(false)
bool(false)
-- Iteration 123 --
bool(false)
bool(false)
bool(false)
-- Iteration 124 --
int(0)
bool(false)
int(0)
-- Iteration 125 --
int(0)
bool(false)
int(0)
-- Iteration 126 --
int(0)
int(0)
int(0)
-- Iteration 127 --
bool(false)
bool(false)
bool(false)
-- Iteration 128 --
bool(false)
bool(false)
bool(false)
-- Iteration 129 --
bool(false)
bool(false)
bool(false)
-- Iteration 130 --
bool(false)
bool(false)
bool(false)
-- Iteration 131 --
bool(false)
bool(false)
bool(false)
-- Iteration 132 --
bool(false)
bool(false)
bool(false)
-- Iteration 133 --
bool(false)
bool(false)
bool(false)
-- Iteration 134 --
bool(false)
bool(false)
bool(false)
-- Iteration 135 --
bool(false)
bool(false)
bool(false)
-- Iteration 136 --
bool(false)
bool(false)
bool(false)
-- Iteration 137 --
bool(false)
bool(false)
bool(false)
-- Iteration 138 --
bool(false)
bool(false)
bool(false)
-- Iteration 139 --
bool(false)
bool(false)
bool(false)
-- Iteration 140 --
bool(false)
bool(false)
bool(false)
-- Iteration 141 --
bool(false)
bool(false)
bool(false)
-- Iteration 142 --
int(0)
bool(false)
int(0)
-- Iteration 143 --
int(0)
bool(false)
int(0)
-- Iteration 144 --
bool(false)
bool(false)
bool(false)
Done
