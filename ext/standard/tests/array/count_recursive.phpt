--TEST--
Test count() function
--FILE--
<?php
echo "*** Testing basic functionality of count() function ***\n";

print "-- Testing arrays --\n";
$arr = array(1, array(3, 4, array(6, array(8))));
print "COUNT_NORMAL: should be 2, is ".count($arr, COUNT_NORMAL)."\n";
print "COUNT_RECURSIVE: should be 8, is ".count($arr, COUNT_RECURSIVE)."\n";

print "-- Testing hashes --\n";
$arr = array("a" => 1, "b" => 2, array("c" => 3, array("d" => 5)));
print "COUNT_NORMAL: should be 3, is ".count($arr, COUNT_NORMAL)."\n";
print "COUNT_RECURSIVE: should be 6, is ".count($arr, COUNT_RECURSIVE)."\n";

print "-- Testing various types with no second argument --\n";
print "COUNT_NORMAL: should be 2, is ".count(array("a", array("b")))."\n";

$arr = array('a'=>array(NULL, NULL, NULL), 1=>array(NULL=>1, 1=>NULL),
    array(array(array(array(array(NULL))))));
print "-- Testing really cool arrays --\n";
print "COUNT_NORMAL: should be 3, is ".count($arr, COUNT_NORMAL)."\n";
print "COUNT_RECURSIVE: should be 13, is ".count($arr, COUNT_RECURSIVE)."\n";

echo "\n*** Testing possible variations of count() function on arrays ***";
$count_array = array(
  array(),
  array( 1 => "string"),
  array( "" => "string", 0 => "a", NULL => "b", -1.00 => "c",
         array(array(array(NULL)))),
  array( -2.44444 => 12, array(array(1, 2, array(array("0"))))),
  array( "a" => 1, "b" => -2.344, "b" => "string", "c" => NULL, "d" => -2.344),
  array( 4 => 1, 3 => -2.344, "3" => "string", "2" => NULL,
         1 => -2.344, array()),
  array( TRUE => TRUE, FALSE => FALSE, "" => "", " " => " ",
     NULL => NULL, "\x000" => "\x000", "\000" => "\000"),
  array( NULL, 1.23 => "Hi", "string" => "hello",
         array("" => "World", "-2.34" => "a", "0" => "b"))
);

$i = 0;
foreach ($count_array as $count_value) {
  echo "\n-- Iteration $i --\n";
  print "COUNT_NORMAL is ".count($count_value, COUNT_NORMAL)."\n";
  print "COUNT_RECURSIVE is ".count($count_value, COUNT_RECURSIVE)."\n";
  $i++;
}

print "\n-- Testing count() on an empty sub-array --\n";
$arr = array(1, array(3, 4, array()));
print "COUNT_NORMAL: should be 2, is ".count($arr, COUNT_NORMAL)."\n";
print "COUNT_RECURSIVE: should be 5, is ".count($arr, COUNT_RECURSIVE)."\n";

echo "\n-- Testing count() on objects with Countable interface --\n";
class count_class implements Countable {
  private $var_private;
  public $var_public;
  protected $var_protected;

  public function count() {
    return 3;
  }
}

$obj = new count_class();
print "COUNT_NORMAL: should be 3, is ".count($obj)."\n";


echo "\n-- Testing count() on resource type --\n";
$resource1 = fopen( __FILE__, "r" );  // Creating file(stream type) resource
$resource2 = opendir( "." );  // Creating dir resource

/* creating an array with resources as elements */
$arr_resource = array("a" => $resource1, "b" => $resource2);
var_dump(count($arr_resource));

echo "\n-- Testing count() on arrays containing references --\n";
$arr = array(1, array("a", "b", "c"));
$arr[2] = &$arr[1];

echo "Count normal" . \PHP_EOL;
var_dump(count($arr, COUNT_NORMAL));
echo "Count recursive" . \PHP_EOL;
var_dump(count($arr, COUNT_RECURSIVE));

/* closing the resource handles */
fclose( $resource1 );
closedir( $resource2 );
?>
--EXPECTF--
*** Testing basic functionality of count() function ***
-- Testing arrays --
COUNT_NORMAL: should be 2, is 2
COUNT_RECURSIVE: should be 8, is 8
-- Testing hashes --
COUNT_NORMAL: should be 3, is 3
COUNT_RECURSIVE: should be 6, is 6
-- Testing various types with no second argument --
COUNT_NORMAL: should be 2, is 2
-- Testing really cool arrays --
COUNT_NORMAL: should be 3, is 3
COUNT_RECURSIVE: should be 13, is 13

*** Testing possible variations of count() function on arrays ***
-- Iteration 0 --
COUNT_NORMAL is 0
COUNT_RECURSIVE is 0

-- Iteration 1 --
COUNT_NORMAL is 1
COUNT_RECURSIVE is 1

-- Iteration 2 --
COUNT_NORMAL is 4
COUNT_RECURSIVE is 7

-- Iteration 3 --
COUNT_NORMAL is 2
COUNT_RECURSIVE is 8

-- Iteration 4 --
COUNT_NORMAL is 4
COUNT_RECURSIVE is 4

-- Iteration 5 --
COUNT_NORMAL is 5
COUNT_RECURSIVE is 5

-- Iteration 6 --
COUNT_NORMAL is 6
COUNT_RECURSIVE is 6

-- Iteration 7 --
COUNT_NORMAL is 4
COUNT_RECURSIVE is 7

-- Testing count() on an empty sub-array --
COUNT_NORMAL: should be 2, is 2
COUNT_RECURSIVE: should be 5, is 5

-- Testing count() on objects with Countable interface --
COUNT_NORMAL: should be 3, is 3

-- Testing count() on resource type --
int(2)

-- Testing count() on arrays containing references --
Count normal
int(3)
Count recursive
int(9)
