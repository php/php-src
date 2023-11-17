--TEST--
Test implode() function
--FILE--
<?php
echo "*** Testing implode() for basic operations ***\n";
$arrays = array (
  array(1,2),
  array(1.1,2.2),
  array(array(2),array(1)),
  array(false,true),
  array(),
  array("a","aaaa","b","bbbb","c","ccccccccccccccccccccc"),
  array(NULL),
);
/* loop to output string with ', ' as $glue, using implode() */
foreach ($arrays as $array) {
  var_dump( implode(', ', $array) );
  var_dump($array);
}

echo "\n*** Testing implode() with variations of glue ***\n";
/* checking possible variations */
$pieces = array (
  2,
  0,
  -639,
  true,
  "PHP",
  false,
  "",
  " ",
  "string\x00with\x00...\0"
);
$glues = array (
  "TRUE",
  true,
  false,
  array("key1", "key2"),
  "",
  " ",
  "string\x00between",
  -0,
  '\0'
);
/* loop through to display a string containing all the array $pieces in the same order,
   with the $glue string between each element  */
$counter = 1;
foreach($glues as $glue) {
  echo "-- Iteration $counter --\n";
  try {
       var_dump(implode($glue, $pieces));
  } catch (TypeError $exception) {
      echo $exception->getMessage() . "\n";
  }
  $counter++;
}

/* empty string */
echo "\n*** Testing implode() on empty string ***\n";
try {
    implode("");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* checking sub-arrays */
echo "\n*** Testing implode() on sub-arrays ***\n";
$sub_array = array(array(1,2,3,4), array(1 => "one", 2 => "two"), "PHP", 50);
var_dump(implode("TEST", $sub_array));
try {
   var_dump(implode(array(1, 2, 3, 4), $sub_array));
} catch (TypeError $exception) {
  echo $exception->getMessage() . "\n";
}
try {
   var_dump( implode(2, $sub_array) );
} catch (TypeError $exception) {
  echo $exception->getMessage() . "\n";
}

echo "\n*** Testing implode() on objects ***\n";
/* checking on objects */
class foo
{
  function __toString() {
    return "Object";
  }
}

$obj = new foo(); //creating new object
$arr = array();
$arr[0] = &$obj;
$arr[1] = &$obj;
var_dump( implode(",", $arr) );
var_dump($arr);

/* Checking on resource types */
echo "\n*** Testing end() on resource type ***\n";
/* file type resource */
$file_handle = fopen(__FILE__, "r");

/* directory type resource */
$dir_handle = opendir( __DIR__ );

/* store resources in array for comparison */
$resources = array($file_handle, $dir_handle);

var_dump( implode("::", $resources) );

/* closing resource handles */
fclose($file_handle);
closedir($dir_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing implode() for basic operations ***
string(4) "1, 2"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
string(8) "1.1, 2.2"
array(2) {
  [0]=>
  float(1.1)
  [1]=>
  float(2.2)
}

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
string(12) "Array, Array"
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}
string(3) ", 1"
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
string(0) ""
array(0) {
}
string(42) "a, aaaa, b, bbbb, c, ccccccccccccccccccccc"
array(6) {
  [0]=>
  string(1) "a"
  [1]=>
  string(4) "aaaa"
  [2]=>
  string(1) "b"
  [3]=>
  string(4) "bbbb"
  [4]=>
  string(1) "c"
  [5]=>
  string(21) "ccccccccccccccccccccc"
}
string(0) ""
array(1) {
  [0]=>
  NULL
}

*** Testing implode() with variations of glue ***
-- Iteration 1 --
string(59) "2TRUE0TRUE-639TRUE1TRUEPHPTRUETRUETRUE TRUEstring%0with%0...%0"
-- Iteration 2 --
string(35) "2101-639111PHP111 1string%0with%0...%0"
-- Iteration 3 --
string(27) "20-6391PHP string%0with%0...%0"
-- Iteration 4 --
implode(): Argument #1 ($separator) must be of type string, array given
-- Iteration 5 --
string(27) "20-6391PHP string%0with%0...%0"
-- Iteration 6 --
string(35) "2 0 -639 1 PHP     string%0with%0...%0"
-- Iteration 7 --
string(139) "2string%0between0string%0between-639string%0between1string%0betweenPHPstring%0betweenstring%0betweenstring%0between string%0betweenstring%0with%0...%0"
-- Iteration 8 --
string(35) "2000-639010PHP000 0string%0with%0...%0"
-- Iteration 9 --
string(43) "2\00\0-639\01\0PHP\0\0\0 \0string%0with%0...%0"

*** Testing implode() on empty string ***
implode(): Argument #2 ($array) must be of type array, null given

*** Testing implode() on sub-arrays ***

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
string(27) "ArrayTESTArrayTESTPHPTEST50"
implode(): Argument #1 ($separator) must be of type string, array given

Warning: Array to string conversion in %s

Warning: Array to string conversion in %s
string(18) "Array2Array2PHP250"

*** Testing implode() on objects ***
string(13) "Object,Object"
array(2) {
  [0]=>
  &object(foo)#%d (0) {
  }
  [1]=>
  &object(foo)#%d (0) {
  }
}

*** Testing end() on resource type ***
string(%d) "Resource id #%d::Resource id #%d"
Done
