--TEST--
Test in_array() function : usage variations - different needdle values
--FILE--
<?php
/* Test in_array() with different possible needle values */

enum Sample {
    case A;
    case B;
}

echo "*** Testing in_array() with different needle values ***\n";
$refVar = "B";
$refVar2 = 4;
$refVar3 = -2.989888;
$refVar4 = NULL;
$refVar5 = Sample::A;
$arrays = [
  [0],
  ["a" => "A", 2 => "B", "C" => 3, 4 => 4, "one" => 1, "" => NULL, "b", "ab", "abcd"],
  [4, [1, 2 => 3], "one" => 1, "5" => 5 ],
  [-1, -2, -3, -4, -2.989888, "-0.005" => "neg0.005", 2 => "float2", "-.9" => -.9],
  [TRUE, FALSE],
  ["", []],
  ["abcd\x00abcd\x00abcd"],
  ["abcd\tabcd\nabcd\rabcd\0abcdefghij"],
  [Sample::A],
];

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
  [],
  NULL,
  "ab",
  "abcd",
  0.0,
  -0,
  "abcd\x00abcd\x00abcd",
  Sample::A,
  Sample::B,
);
/* loop to check if elements in $array_compare exist in $arrays
   using in_array() */
$counter = 1;

function get_type(mixed $value): string {
    if (is_object($value)) {
        return var_export($value, true);
    }
    return gettype($value). ':' . json_encode($value, true);
}
foreach($arrays as $arrayKey => $array) {
  foreach($array_compare as $compare) {
    echo "-- Iteration $counter -- array ".$arrayKey." -- ".get_type($compare)."\n";
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
-- Iteration 1 -- array 0 -- integer:4
bool(false)
bool(false)
bool(false)
-- Iteration 2 -- array 0 -- string:"4"
bool(false)
bool(false)
bool(false)
-- Iteration 3 -- array 0 -- double:4
bool(false)
bool(false)
bool(false)
-- Iteration 4 -- array 0 -- string:"b"
bool(false)
bool(false)
bool(false)
-- Iteration 5 -- array 0 -- string:"5"
bool(false)
bool(false)
bool(false)
-- Iteration 6 -- array 0 -- integer:-2
bool(false)
bool(false)
bool(false)
-- Iteration 7 -- array 0 -- double:-2
bool(false)
bool(false)
bool(false)
-- Iteration 8 -- array 0 -- double:-2.98989
bool(false)
bool(false)
bool(false)
-- Iteration 9 -- array 0 -- string:"-.9"
bool(false)
bool(false)
bool(false)
-- Iteration 10 -- array 0 -- string:"True"
bool(false)
bool(false)
bool(false)
-- Iteration 11 -- array 0 -- string:""
bool(false)
bool(false)
bool(false)
-- Iteration 12 -- array 0 -- array:[]
bool(false)
bool(false)
bool(false)
-- Iteration 13 -- array 0 -- NULL:null
bool(true)
bool(false)
bool(true)
-- Iteration 14 -- array 0 -- string:"ab"
bool(false)
bool(false)
bool(false)
-- Iteration 15 -- array 0 -- string:"abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 16 -- array 0 -- double:0
bool(true)
bool(false)
bool(true)
-- Iteration 17 -- array 0 -- integer:0
bool(true)
bool(true)
bool(true)
-- Iteration 18 -- array 0 -- string:"abcd\u0000abcd\u0000abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 19 -- array 0 -- \Sample::A
bool(false)
bool(false)
bool(false)
-- Iteration 20 -- array 0 -- \Sample::B
bool(false)
bool(false)
bool(false)
-- Iteration 21 -- array 1 -- integer:4
bool(true)
bool(true)
bool(true)
-- Iteration 22 -- array 1 -- string:"4"
bool(true)
bool(false)
bool(true)
-- Iteration 23 -- array 1 -- double:4
bool(true)
bool(false)
bool(true)
-- Iteration 24 -- array 1 -- string:"b"
bool(true)
bool(true)
bool(true)
-- Iteration 25 -- array 1 -- string:"5"
bool(false)
bool(false)
bool(false)
-- Iteration 26 -- array 1 -- integer:-2
bool(false)
bool(false)
bool(false)
-- Iteration 27 -- array 1 -- double:-2
bool(false)
bool(false)
bool(false)
-- Iteration 28 -- array 1 -- double:-2.98989
bool(false)
bool(false)
bool(false)
-- Iteration 29 -- array 1 -- string:"-.9"
bool(false)
bool(false)
bool(false)
-- Iteration 30 -- array 1 -- string:"True"
bool(false)
bool(false)
bool(false)
-- Iteration 31 -- array 1 -- string:""
bool(true)
bool(false)
bool(true)
-- Iteration 32 -- array 1 -- array:[]
bool(true)
bool(false)
bool(true)
-- Iteration 33 -- array 1 -- NULL:null
bool(true)
bool(true)
bool(true)
-- Iteration 34 -- array 1 -- string:"ab"
bool(true)
bool(true)
bool(true)
-- Iteration 35 -- array 1 -- string:"abcd"
bool(true)
bool(true)
bool(true)
-- Iteration 36 -- array 1 -- double:0
bool(true)
bool(false)
bool(true)
-- Iteration 37 -- array 1 -- integer:0
bool(true)
bool(false)
bool(true)
-- Iteration 38 -- array 1 -- string:"abcd\u0000abcd\u0000abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 39 -- array 1 -- \Sample::A
bool(false)
bool(false)
bool(false)
-- Iteration 40 -- array 1 -- \Sample::B
bool(false)
bool(false)
bool(false)
-- Iteration 41 -- array 2 -- integer:4
bool(true)
bool(true)
bool(true)
-- Iteration 42 -- array 2 -- string:"4"
bool(true)
bool(false)
bool(true)
-- Iteration 43 -- array 2 -- double:4
bool(true)
bool(false)
bool(true)
-- Iteration 44 -- array 2 -- string:"b"
bool(false)
bool(false)
bool(false)
-- Iteration 45 -- array 2 -- string:"5"
bool(true)
bool(false)
bool(true)
-- Iteration 46 -- array 2 -- integer:-2
bool(false)
bool(false)
bool(false)
-- Iteration 47 -- array 2 -- double:-2
bool(false)
bool(false)
bool(false)
-- Iteration 48 -- array 2 -- double:-2.98989
bool(false)
bool(false)
bool(false)
-- Iteration 49 -- array 2 -- string:"-.9"
bool(false)
bool(false)
bool(false)
-- Iteration 50 -- array 2 -- string:"True"
bool(false)
bool(false)
bool(false)
-- Iteration 51 -- array 2 -- string:""
bool(false)
bool(false)
bool(false)
-- Iteration 52 -- array 2 -- array:[]
bool(false)
bool(false)
bool(false)
-- Iteration 53 -- array 2 -- NULL:null
bool(false)
bool(false)
bool(false)
-- Iteration 54 -- array 2 -- string:"ab"
bool(false)
bool(false)
bool(false)
-- Iteration 55 -- array 2 -- string:"abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 56 -- array 2 -- double:0
bool(false)
bool(false)
bool(false)
-- Iteration 57 -- array 2 -- integer:0
bool(false)
bool(false)
bool(false)
-- Iteration 58 -- array 2 -- string:"abcd\u0000abcd\u0000abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 59 -- array 2 -- \Sample::A
bool(false)
bool(false)
bool(false)
-- Iteration 60 -- array 2 -- \Sample::B
bool(false)
bool(false)
bool(false)
-- Iteration 61 -- array 3 -- integer:4
bool(false)
bool(false)
bool(false)
-- Iteration 62 -- array 3 -- string:"4"
bool(false)
bool(false)
bool(false)
-- Iteration 63 -- array 3 -- double:4
bool(false)
bool(false)
bool(false)
-- Iteration 64 -- array 3 -- string:"b"
bool(false)
bool(false)
bool(false)
-- Iteration 65 -- array 3 -- string:"5"
bool(false)
bool(false)
bool(false)
-- Iteration 66 -- array 3 -- integer:-2
bool(true)
bool(true)
bool(true)
-- Iteration 67 -- array 3 -- double:-2
bool(true)
bool(false)
bool(true)
-- Iteration 68 -- array 3 -- double:-2.98989
bool(false)
bool(false)
bool(false)
-- Iteration 69 -- array 3 -- string:"-.9"
bool(true)
bool(false)
bool(true)
-- Iteration 70 -- array 3 -- string:"True"
bool(false)
bool(false)
bool(false)
-- Iteration 71 -- array 3 -- string:""
bool(false)
bool(false)
bool(false)
-- Iteration 72 -- array 3 -- array:[]
bool(false)
bool(false)
bool(false)
-- Iteration 73 -- array 3 -- NULL:null
bool(false)
bool(false)
bool(false)
-- Iteration 74 -- array 3 -- string:"ab"
bool(false)
bool(false)
bool(false)
-- Iteration 75 -- array 3 -- string:"abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 76 -- array 3 -- double:0
bool(false)
bool(false)
bool(false)
-- Iteration 77 -- array 3 -- integer:0
bool(false)
bool(false)
bool(false)
-- Iteration 78 -- array 3 -- string:"abcd\u0000abcd\u0000abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 79 -- array 3 -- \Sample::A
bool(false)
bool(false)
bool(false)
-- Iteration 80 -- array 3 -- \Sample::B
bool(false)
bool(false)
bool(false)
-- Iteration 81 -- array 4 -- integer:4
bool(true)
bool(false)
bool(true)
-- Iteration 82 -- array 4 -- string:"4"
bool(true)
bool(false)
bool(true)
-- Iteration 83 -- array 4 -- double:4
bool(true)
bool(false)
bool(true)
-- Iteration 84 -- array 4 -- string:"b"
bool(true)
bool(false)
bool(true)
-- Iteration 85 -- array 4 -- string:"5"
bool(true)
bool(false)
bool(true)
-- Iteration 86 -- array 4 -- integer:-2
bool(true)
bool(false)
bool(true)
-- Iteration 87 -- array 4 -- double:-2
bool(true)
bool(false)
bool(true)
-- Iteration 88 -- array 4 -- double:-2.98989
bool(true)
bool(false)
bool(true)
-- Iteration 89 -- array 4 -- string:"-.9"
bool(true)
bool(false)
bool(true)
-- Iteration 90 -- array 4 -- string:"True"
bool(true)
bool(false)
bool(true)
-- Iteration 91 -- array 4 -- string:""
bool(true)
bool(false)
bool(true)
-- Iteration 92 -- array 4 -- array:[]
bool(true)
bool(false)
bool(true)
-- Iteration 93 -- array 4 -- NULL:null
bool(true)
bool(false)
bool(true)
-- Iteration 94 -- array 4 -- string:"ab"
bool(true)
bool(false)
bool(true)
-- Iteration 95 -- array 4 -- string:"abcd"
bool(true)
bool(false)
bool(true)
-- Iteration 96 -- array 4 -- double:0
bool(true)
bool(false)
bool(true)
-- Iteration 97 -- array 4 -- integer:0
bool(true)
bool(false)
bool(true)
-- Iteration 98 -- array 4 -- string:"abcd\u0000abcd\u0000abcd"
bool(true)
bool(false)
bool(true)
-- Iteration 99 -- array 4 -- \Sample::A
bool(true)
bool(false)
bool(true)
-- Iteration 100 -- array 4 -- \Sample::B
bool(true)
bool(false)
bool(true)
-- Iteration 101 -- array 5 -- integer:4
bool(false)
bool(false)
bool(false)
-- Iteration 102 -- array 5 -- string:"4"
bool(false)
bool(false)
bool(false)
-- Iteration 103 -- array 5 -- double:4
bool(false)
bool(false)
bool(false)
-- Iteration 104 -- array 5 -- string:"b"
bool(false)
bool(false)
bool(false)
-- Iteration 105 -- array 5 -- string:"5"
bool(false)
bool(false)
bool(false)
-- Iteration 106 -- array 5 -- integer:-2
bool(false)
bool(false)
bool(false)
-- Iteration 107 -- array 5 -- double:-2
bool(false)
bool(false)
bool(false)
-- Iteration 108 -- array 5 -- double:-2.98989
bool(false)
bool(false)
bool(false)
-- Iteration 109 -- array 5 -- string:"-.9"
bool(false)
bool(false)
bool(false)
-- Iteration 110 -- array 5 -- string:"True"
bool(false)
bool(false)
bool(false)
-- Iteration 111 -- array 5 -- string:""
bool(true)
bool(true)
bool(true)
-- Iteration 112 -- array 5 -- array:[]
bool(true)
bool(true)
bool(true)
-- Iteration 113 -- array 5 -- NULL:null
bool(true)
bool(false)
bool(true)
-- Iteration 114 -- array 5 -- string:"ab"
bool(false)
bool(false)
bool(false)
-- Iteration 115 -- array 5 -- string:"abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 116 -- array 5 -- double:0
bool(false)
bool(false)
bool(false)
-- Iteration 117 -- array 5 -- integer:0
bool(false)
bool(false)
bool(false)
-- Iteration 118 -- array 5 -- string:"abcd\u0000abcd\u0000abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 119 -- array 5 -- \Sample::A
bool(false)
bool(false)
bool(false)
-- Iteration 120 -- array 5 -- \Sample::B
bool(false)
bool(false)
bool(false)
-- Iteration 121 -- array 6 -- integer:4
bool(false)
bool(false)
bool(false)
-- Iteration 122 -- array 6 -- string:"4"
bool(false)
bool(false)
bool(false)
-- Iteration 123 -- array 6 -- double:4
bool(false)
bool(false)
bool(false)
-- Iteration 124 -- array 6 -- string:"b"
bool(false)
bool(false)
bool(false)
-- Iteration 125 -- array 6 -- string:"5"
bool(false)
bool(false)
bool(false)
-- Iteration 126 -- array 6 -- integer:-2
bool(false)
bool(false)
bool(false)
-- Iteration 127 -- array 6 -- double:-2
bool(false)
bool(false)
bool(false)
-- Iteration 128 -- array 6 -- double:-2.98989
bool(false)
bool(false)
bool(false)
-- Iteration 129 -- array 6 -- string:"-.9"
bool(false)
bool(false)
bool(false)
-- Iteration 130 -- array 6 -- string:"True"
bool(false)
bool(false)
bool(false)
-- Iteration 131 -- array 6 -- string:""
bool(false)
bool(false)
bool(false)
-- Iteration 132 -- array 6 -- array:[]
bool(false)
bool(false)
bool(false)
-- Iteration 133 -- array 6 -- NULL:null
bool(false)
bool(false)
bool(false)
-- Iteration 134 -- array 6 -- string:"ab"
bool(false)
bool(false)
bool(false)
-- Iteration 135 -- array 6 -- string:"abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 136 -- array 6 -- double:0
bool(false)
bool(false)
bool(false)
-- Iteration 137 -- array 6 -- integer:0
bool(false)
bool(false)
bool(false)
-- Iteration 138 -- array 6 -- string:"abcd\u0000abcd\u0000abcd"
bool(true)
bool(true)
bool(true)
-- Iteration 139 -- array 6 -- \Sample::A
bool(false)
bool(false)
bool(false)
-- Iteration 140 -- array 6 -- \Sample::B
bool(false)
bool(false)
bool(false)
-- Iteration 141 -- array 7 -- integer:4
bool(false)
bool(false)
bool(false)
-- Iteration 142 -- array 7 -- string:"4"
bool(false)
bool(false)
bool(false)
-- Iteration 143 -- array 7 -- double:4
bool(false)
bool(false)
bool(false)
-- Iteration 144 -- array 7 -- string:"b"
bool(false)
bool(false)
bool(false)
-- Iteration 145 -- array 7 -- string:"5"
bool(false)
bool(false)
bool(false)
-- Iteration 146 -- array 7 -- integer:-2
bool(false)
bool(false)
bool(false)
-- Iteration 147 -- array 7 -- double:-2
bool(false)
bool(false)
bool(false)
-- Iteration 148 -- array 7 -- double:-2.98989
bool(false)
bool(false)
bool(false)
-- Iteration 149 -- array 7 -- string:"-.9"
bool(false)
bool(false)
bool(false)
-- Iteration 150 -- array 7 -- string:"True"
bool(false)
bool(false)
bool(false)
-- Iteration 151 -- array 7 -- string:""
bool(false)
bool(false)
bool(false)
-- Iteration 152 -- array 7 -- array:[]
bool(false)
bool(false)
bool(false)
-- Iteration 153 -- array 7 -- NULL:null
bool(false)
bool(false)
bool(false)
-- Iteration 154 -- array 7 -- string:"ab"
bool(false)
bool(false)
bool(false)
-- Iteration 155 -- array 7 -- string:"abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 156 -- array 7 -- double:0
bool(false)
bool(false)
bool(false)
-- Iteration 157 -- array 7 -- integer:0
bool(false)
bool(false)
bool(false)
-- Iteration 158 -- array 7 -- string:"abcd\u0000abcd\u0000abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 159 -- array 7 -- \Sample::A
bool(false)
bool(false)
bool(false)
-- Iteration 160 -- array 7 -- \Sample::B
bool(false)
bool(false)
bool(false)
-- Iteration 161 -- array 8 -- integer:4
bool(false)
bool(false)
bool(false)
-- Iteration 162 -- array 8 -- string:"4"
bool(false)
bool(false)
bool(false)
-- Iteration 163 -- array 8 -- double:4
bool(false)
bool(false)
bool(false)
-- Iteration 164 -- array 8 -- string:"b"
bool(false)
bool(false)
bool(false)
-- Iteration 165 -- array 8 -- string:"5"
bool(false)
bool(false)
bool(false)
-- Iteration 166 -- array 8 -- integer:-2
bool(false)
bool(false)
bool(false)
-- Iteration 167 -- array 8 -- double:-2
bool(false)
bool(false)
bool(false)
-- Iteration 168 -- array 8 -- double:-2.98989
bool(false)
bool(false)
bool(false)
-- Iteration 169 -- array 8 -- string:"-.9"
bool(false)
bool(false)
bool(false)
-- Iteration 170 -- array 8 -- string:"True"
bool(false)
bool(false)
bool(false)
-- Iteration 171 -- array 8 -- string:""
bool(false)
bool(false)
bool(false)
-- Iteration 172 -- array 8 -- array:[]
bool(false)
bool(false)
bool(false)
-- Iteration 173 -- array 8 -- NULL:null
bool(false)
bool(false)
bool(false)
-- Iteration 174 -- array 8 -- string:"ab"
bool(false)
bool(false)
bool(false)
-- Iteration 175 -- array 8 -- string:"abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 176 -- array 8 -- double:0
bool(false)
bool(false)
bool(false)
-- Iteration 177 -- array 8 -- integer:0
bool(false)
bool(false)
bool(false)
-- Iteration 178 -- array 8 -- string:"abcd\u0000abcd\u0000abcd"
bool(false)
bool(false)
bool(false)
-- Iteration 179 -- array 8 -- \Sample::A
bool(true)
bool(true)
bool(true)
-- Iteration 180 -- array 8 -- \Sample::B
bool(false)
bool(false)
bool(false)
Done
