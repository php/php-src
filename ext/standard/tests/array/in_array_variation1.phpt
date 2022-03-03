--TEST--
Test in_array() function : usage variations - different needdle values
--FILE--
<?php
/* Test in_array() with different possible needle values */

echo "*** Testing in_array() with different needle values ***\n";
$arrays = array (
  array(0),
  array("a" => "A", 2 => "B", "C" => 3, 4 => 4, "one" => 1, "" => NULL, "b", "ab", "abcd"),
  array(4, array(1, 2 => 3), "one" => 1, "5" => 5 ),
  array(-1, -2, -3, -4, -2.989888, "-0.005" => "neg0.005", 2 => "float2", "-.9" => -.9),
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
   using in_array() */
$counter = 1;
foreach($arrays as $array) {
  foreach($array_compare as $compare) {
    echo "-- Iteration $counter --\n";
    //strict option OFF
    var_dump(in_array($compare,$array));
    //strict option ON
    var_dump(in_array($compare,$array,TRUE));
    //strict option OFF
    var_dump(in_array($compare,$array,FALSE));
    $counter++;
 }
}

echo "Done\n";
?>
--EXPECT--
*** Testing in_array() with different needle values ***
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
bool(false)
bool(false)
bool(false)
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
bool(false)
bool(false)
bool(false)
-- Iteration 11 --
bool(false)
bool(false)
bool(false)
-- Iteration 12 --
bool(false)
bool(false)
bool(false)
-- Iteration 13 --
bool(true)
bool(false)
bool(true)
-- Iteration 14 --
bool(false)
bool(false)
bool(false)
-- Iteration 15 --
bool(false)
bool(false)
bool(false)
-- Iteration 16 --
bool(true)
bool(false)
bool(true)
-- Iteration 17 --
bool(true)
bool(true)
bool(true)
-- Iteration 18 --
bool(false)
bool(false)
bool(false)
-- Iteration 19 --
bool(true)
bool(true)
bool(true)
-- Iteration 20 --
bool(true)
bool(false)
bool(true)
-- Iteration 21 --
bool(true)
bool(false)
bool(true)
-- Iteration 22 --
bool(true)
bool(true)
bool(true)
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
bool(true)
bool(false)
bool(true)
-- Iteration 30 --
bool(true)
bool(false)
bool(true)
-- Iteration 31 --
bool(true)
bool(true)
bool(true)
-- Iteration 32 --
bool(true)
bool(true)
bool(true)
-- Iteration 33 --
bool(true)
bool(true)
bool(true)
-- Iteration 34 --
bool(true)
bool(false)
bool(true)
-- Iteration 35 --
bool(true)
bool(false)
bool(true)
-- Iteration 36 --
bool(false)
bool(false)
bool(false)
-- Iteration 37 --
bool(true)
bool(true)
bool(true)
-- Iteration 38 --
bool(true)
bool(false)
bool(true)
-- Iteration 39 --
bool(true)
bool(false)
bool(true)
-- Iteration 40 --
bool(false)
bool(false)
bool(false)
-- Iteration 41 --
bool(true)
bool(false)
bool(true)
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
bool(true)
bool(true)
bool(true)
-- Iteration 61 --
bool(true)
bool(false)
bool(true)
-- Iteration 62 --
bool(false)
bool(false)
bool(false)
-- Iteration 63 --
bool(true)
bool(false)
bool(true)
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
bool(false)
bool(false)
bool(false)
-- Iteration 71 --
bool(false)
bool(false)
bool(false)
-- Iteration 72 --
bool(false)
bool(false)
bool(false)
-- Iteration 73 --
bool(true)
bool(false)
bool(true)
-- Iteration 74 --
bool(true)
bool(false)
bool(true)
-- Iteration 75 --
bool(true)
bool(false)
bool(true)
-- Iteration 76 --
bool(true)
bool(false)
bool(true)
-- Iteration 77 --
bool(true)
bool(false)
bool(true)
-- Iteration 78 --
bool(true)
bool(false)
bool(true)
-- Iteration 79 --
bool(true)
bool(false)
bool(true)
-- Iteration 80 --
bool(true)
bool(false)
bool(true)
-- Iteration 81 --
bool(true)
bool(false)
bool(true)
-- Iteration 82 --
bool(true)
bool(false)
bool(true)
-- Iteration 83 --
bool(true)
bool(false)
bool(true)
-- Iteration 84 --
bool(true)
bool(false)
bool(true)
-- Iteration 85 --
bool(true)
bool(false)
bool(true)
-- Iteration 86 --
bool(true)
bool(false)
bool(true)
-- Iteration 87 --
bool(true)
bool(false)
bool(true)
-- Iteration 88 --
bool(true)
bool(false)
bool(true)
-- Iteration 89 --
bool(true)
bool(false)
bool(true)
-- Iteration 90 --
bool(true)
bool(false)
bool(true)
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
bool(true)
bool(true)
bool(true)
-- Iteration 102 --
bool(true)
bool(true)
bool(true)
-- Iteration 103 --
bool(true)
bool(false)
bool(true)
-- Iteration 104 --
bool(false)
bool(false)
bool(false)
-- Iteration 105 --
bool(false)
bool(false)
bool(false)
-- Iteration 106 --
bool(false)
bool(false)
bool(false)
-- Iteration 107 --
bool(false)
bool(false)
bool(false)
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
bool(false)
bool(false)
bool(false)
-- Iteration 125 --
bool(false)
bool(false)
bool(false)
-- Iteration 126 --
bool(true)
bool(true)
bool(true)
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
bool(false)
bool(false)
bool(false)
-- Iteration 143 --
bool(false)
bool(false)
bool(false)
-- Iteration 144 --
bool(false)
bool(false)
bool(false)
Done
