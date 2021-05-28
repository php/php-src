--TEST--
Test serialize() & unserialize() functions: many types
--INI--
serialize_precision=17
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
    die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php
echo "--- Testing Various Types ---\n";

/* unset variable */
$unset_var = 10;
unset($unset_var);
/* array declaration */
$arr_var = array(0, 1, -2, 3.333333, "a", array(), array(NULL));

$Variation_arr = array(
   /* Integers */
   2147483647,
   -2147483647,
   2147483648,
   -2147483648,

   0xFF00123,  // hex integers
   -0xFF00123,
   0x7FFFFFFF,
   -0x7FFFFFFF,
   0x80000000,
   -0x80000000,

   01234567,  // octal integers
   -01234567,

   /* arrays */
   array(),  // zero elements
   array(1, 2, 3, 12345666, -2344),
   array(0, 1, 2, 3.333, -4, -5.555, TRUE, FALSE, NULL, "", '', " ",
         array(), array(1,2,array()), "string", new stdclass
        ),
   &$arr_var,  // Reference to an array

  /* nulls */
   NULL,
   null,

  /* strings */
   "",
   '',
   " ",
   ' ',
   "a",
   "string",
   'string',
   "hello\0",
   'hello\0',
   "123",
   '123',
   '\t',
   "\t",

   /* booleans */
   TRUE,
   true,
   FALSE,
   false,

   /* Mixed types */
   "TRUE123",
   "123string",
   "string123",
   "NULLstring",

   /* unset/undefined  vars */
   @$unset_var,
   @$undefined_var,
);

/* Loop through to test each element in the above array */
for( $i = 0; $i < count($Variation_arr); $i++ ) {

  echo "\n-- Iteration $i --\n";
  echo "after serialization => ";
  $serialize_data = serialize($Variation_arr[$i]);
  var_dump( $serialize_data );

  echo "after unserialization => ";
  $unserialize_data = unserialize($serialize_data);
  var_dump( $unserialize_data );
}

echo "\nDone";
?>
--EXPECTF--
--- Testing Various Types ---

-- Iteration 0 --
after serialization => string(13) "i:2147483647;"
after unserialization => int(2147483647)

-- Iteration 1 --
after serialization => string(14) "i:-2147483647;"
after unserialization => int(-2147483647)

-- Iteration 2 --
after serialization => string(13) "d:2147483648;"
after unserialization => float(2147483648)

-- Iteration 3 --
after serialization => string(14) "d:-2147483648;"
after unserialization => float(-2147483648)

-- Iteration 4 --
after serialization => string(12) "i:267387171;"
after unserialization => int(267387171)

-- Iteration 5 --
after serialization => string(13) "i:-267387171;"
after unserialization => int(-267387171)

-- Iteration 6 --
after serialization => string(13) "i:2147483647;"
after unserialization => int(2147483647)

-- Iteration 7 --
after serialization => string(14) "i:-2147483647;"
after unserialization => int(-2147483647)

-- Iteration 8 --
after serialization => string(13) "d:2147483648;"
after unserialization => float(2147483648)

-- Iteration 9 --
after serialization => string(14) "d:-2147483648;"
after unserialization => float(-2147483648)

-- Iteration 10 --
after serialization => string(9) "i:342391;"
after unserialization => int(342391)

-- Iteration 11 --
after serialization => string(10) "i:-342391;"
after unserialization => int(-342391)

-- Iteration 12 --
after serialization => string(6) "a:0:{}"
after unserialization => array(0) {
}

-- Iteration 13 --
after serialization => string(57) "a:5:{i:0;i:1;i:1;i:2;i:2;i:3;i:3;i:12345666;i:4;i:-2344;}"
after unserialization => array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(12345666)
  [4]=>
  int(-2344)
}

-- Iteration 14 --
after serialization => string(239) "a:16:{i:0;i:0;i:1;i:1;i:2;i:2;i:3;d:3.3330000000000002;i:4;i:-4;i:5;d:-5.5549999999999997;i:6;b:1;i:7;b:0;i:8;N;i:9;s:0:"";i:10;s:0:"";i:11;s:1:" ";i:12;a:0:{}i:13;a:3:{i:0;i:1;i:1;i:2;i:2;a:0:{}}i:14;s:6:"string";i:15;O:8:"stdClass":0:{}}"
after unserialization => array(16) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  float(3.3330000000000002)
  [4]=>
  int(-4)
  [5]=>
  float(-5.5549999999999997)
  [6]=>
  bool(true)
  [7]=>
  bool(false)
  [8]=>
  NULL
  [9]=>
  string(0) ""
  [10]=>
  string(0) ""
  [11]=>
  string(1) " "
  [12]=>
  array(0) {
  }
  [13]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    array(0) {
    }
  }
  [14]=>
  string(6) "string"
  [15]=>
  object(stdClass)#2 (0) {
  }
}

-- Iteration 15 --
after serialization => string(94) "a:7:{i:0;i:0;i:1;i:1;i:2;i:-2;i:3;d:3.3333330000000001;i:4;s:1:"a";i:5;a:0:{}i:6;a:1:{i:0;N;}}"
after unserialization => array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(-2)
  [3]=>
  float(3.3333330000000001)
  [4]=>
  string(1) "a"
  [5]=>
  array(0) {
  }
  [6]=>
  array(1) {
    [0]=>
    NULL
  }
}

-- Iteration 16 --
after serialization => string(2) "N;"
after unserialization => NULL

-- Iteration 17 --
after serialization => string(2) "N;"
after unserialization => NULL

-- Iteration 18 --
after serialization => string(7) "s:0:"";"
after unserialization => string(0) ""

-- Iteration 19 --
after serialization => string(7) "s:0:"";"
after unserialization => string(0) ""

-- Iteration 20 --
after serialization => string(8) "s:1:" ";"
after unserialization => string(1) " "

-- Iteration 21 --
after serialization => string(8) "s:1:" ";"
after unserialization => string(1) " "

-- Iteration 22 --
after serialization => string(8) "s:1:"a";"
after unserialization => string(1) "a"

-- Iteration 23 --
after serialization => string(13) "s:6:"string";"
after unserialization => string(6) "string"

-- Iteration 24 --
after serialization => string(13) "s:6:"string";"
after unserialization => string(6) "string"

-- Iteration 25 --
after serialization => string(13) "s:6:"hello%0";"
after unserialization => string(6) "hello%0"

-- Iteration 26 --
after serialization => string(14) "s:7:"hello\0";"
after unserialization => string(7) "hello\0"

-- Iteration 27 --
after serialization => string(10) "s:3:"123";"
after unserialization => string(3) "123"

-- Iteration 28 --
after serialization => string(10) "s:3:"123";"
after unserialization => string(3) "123"

-- Iteration 29 --
after serialization => string(9) "s:2:"\t";"
after unserialization => string(2) "\t"

-- Iteration 30 --
after serialization => string(8) "s:1:"	";"
after unserialization => string(1) "	"

-- Iteration 31 --
after serialization => string(4) "b:1;"
after unserialization => bool(true)

-- Iteration 32 --
after serialization => string(4) "b:1;"
after unserialization => bool(true)

-- Iteration 33 --
after serialization => string(4) "b:0;"
after unserialization => bool(false)

-- Iteration 34 --
after serialization => string(4) "b:0;"
after unserialization => bool(false)

-- Iteration 35 --
after serialization => string(14) "s:7:"TRUE123";"
after unserialization => string(7) "TRUE123"

-- Iteration 36 --
after serialization => string(16) "s:9:"123string";"
after unserialization => string(9) "123string"

-- Iteration 37 --
after serialization => string(16) "s:9:"string123";"
after unserialization => string(9) "string123"

-- Iteration 38 --
after serialization => string(18) "s:10:"NULLstring";"
after unserialization => string(10) "NULLstring"

-- Iteration 39 --
after serialization => string(2) "N;"
after unserialization => NULL

-- Iteration 40 --
after serialization => string(2) "N;"
after unserialization => NULL

Done
