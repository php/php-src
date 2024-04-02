--TEST--
Test debug_zval_dump() function : basic operations
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
precision=14
opcache.enable=0
--FILE--
<?php

/* creating file resource */
$file_handle = fopen(__FILE__, "r");

echo "*** Testing debug_zval_dump() on scalar and non-scalar variables ***\n";
$values = array (
  /* integers */
  0,  // zero as argument
  000000123,  //octal value of 83
  123000000,
  -00000123,  //octal value of 83
  -12300000,
  0xffffff,  //hexadecimal value
  123456789,
  1,
  -1,

  /* floats */
  -0.0,
  +0.0,
  1.234,
  -1.234,
  -2.000000,
  2.0000000,
  -4.0001e+5,
  4.0001E+5,
  6.99999989,
  -.5,
  .567,
  -.6700000e-3,
  -.6700000E+3,
  1E-5,
  -1e+5,
  1e+5,
  1E-5,

  /* strings */
  "",
  '',
  " ",
  ' ',
  "0",
  "\0",
  '\0',
  "\t",
  '\t',
  "PHP",
  'PHP',
  "1234\t\n5678\n\t9100\rabcda\x0000cdeh\0stuv",  // strings with escape chars

  /* boolean */
  TRUE,
  FALSE,
  true,
  false,

  /* arrays */
  array(),
  array(NULL),
  array(true),
  array(""),
  array(''),
  array(array(1, 2), array('a', 'b')),
  array("test" => "is_array", 1 => 'One'),
  array(0),
  array(-1),
  array(10.5, 5.6),
  array("string", "test"),
  array('string', 'test'),

  /* resources */
  $file_handle
);
/* loop to display the variables and its reference count using
    debug_zval_dump() */
$counter = 1;
foreach( $values as $value ) {
  echo "-- Iteration $counter --\n";
  debug_zval_dump( $value );
  $counter++;
}

/* closing resource handle */
fclose($file_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing debug_zval_dump() on scalar and non-scalar variables ***
-- Iteration 1 --
int(0)
-- Iteration 2 --
int(83)
-- Iteration 3 --
int(123000000)
-- Iteration 4 --
int(-83)
-- Iteration 5 --
int(-12300000)
-- Iteration 6 --
int(16777215)
-- Iteration 7 --
int(123456789)
-- Iteration 8 --
int(1)
-- Iteration 9 --
int(-1)
-- Iteration 10 --
float(-0)
-- Iteration 11 --
float(0)
-- Iteration 12 --
float(1.234)
-- Iteration 13 --
float(-1.234)
-- Iteration 14 --
float(-2)
-- Iteration 15 --
float(2)
-- Iteration 16 --
float(-400010)
-- Iteration 17 --
float(400010)
-- Iteration 18 --
float(6.99999989)
-- Iteration 19 --
float(-0.5)
-- Iteration 20 --
float(0.567)
-- Iteration 21 --
float(-0.00067)
-- Iteration 22 --
float(-670)
-- Iteration 23 --
float(1.0E-5)
-- Iteration 24 --
float(-100000)
-- Iteration 25 --
float(100000)
-- Iteration 26 --
float(1.0E-5)
-- Iteration 27 --
string(0) "" interned
-- Iteration 28 --
string(0) "" interned
-- Iteration 29 --
string(1) " " interned
-- Iteration 30 --
string(1) " " interned
-- Iteration 31 --
string(1) "0" interned
-- Iteration 32 --
string(1) "%0" interned
-- Iteration 33 --
string(2) "\0" interned
-- Iteration 34 --
string(1) "	" interned
-- Iteration 35 --
string(2) "\t" interned
-- Iteration 36 --
string(3) "PHP" interned
-- Iteration 37 --
string(3) "PHP" interned
-- Iteration 38 --
string(34) "1234	
5678
	9100abcda%000cdeh%0stuv" interned
-- Iteration 39 --
bool(true)
-- Iteration 40 --
bool(false)
-- Iteration 41 --
bool(true)
-- Iteration 42 --
bool(false)
-- Iteration 43 --
array(0) interned {
}
-- Iteration 44 --
array(1) packed refcount(%d){
  [0]=>
  NULL
}
-- Iteration 45 --
array(1) packed refcount(%d){
  [0]=>
  bool(true)
}
-- Iteration 46 --
array(1) packed refcount(%d){
  [0]=>
  string(0) "" interned
}
-- Iteration 47 --
array(1) packed refcount(%d){
  [0]=>
  string(0) "" interned
}
-- Iteration 48 --
array(2) packed refcount(%d){
  [0]=>
  array(2) packed refcount(%d){
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) packed refcount(%d){
    [0]=>
    string(1) "a" interned
    [1]=>
    string(1) "b" interned
  }
}
-- Iteration 49 --
array(2) refcount(%d){
  ["test"]=>
  string(8) "is_array" refcount(%d)
  [1]=>
  string(3) "One" refcount(%d)
}
-- Iteration 50 --
array(1) packed refcount(%d){
  [0]=>
  int(0)
}
-- Iteration 51 --
array(1) packed refcount(%d){
  [0]=>
  int(-1)
}
-- Iteration 52 --
array(2) packed refcount(%d){
  [0]=>
  float(10.5)
  [1]=>
  float(5.6)
}
-- Iteration 53 --
array(2) packed refcount(%d){
  [0]=>
  string(6) "string" refcount(%d)
  [1]=>
  string(4) "test" refcount(%d)
}
-- Iteration 54 --
array(2) packed refcount(%d){
  [0]=>
  string(6) "string" refcount(%d)
  [1]=>
  string(4) "test" refcount(%d)
}
-- Iteration 55 --
resource(%d) of type (stream) refcount(%d)
Done
