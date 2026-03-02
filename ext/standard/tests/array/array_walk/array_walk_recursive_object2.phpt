--TEST--
Test array_walk_recursive() function : object functionality - array of objects
--FILE--
<?php
/*
* Testing array_walk_recursive() with an array of objects
*/

echo "*** Testing array_walk_recursive() : array of objects ***\n";

function callback_private($value, $key, $addValue)
{
  echo "value : ";
  var_dump($value->getValue());
  echo "key : ";
  var_dump($key);
}

function callback_public($value, $key)
{
  echo "value : ";
  var_dump($value->pub_value);
}
function callback_protected($value, $key)
{
  echo "value : ";
  var_dump($value->get_pro_value());
}

class MyClass
{
  private $pri_value;
  public $pub_value;
  protected $pro_value;
  public function __construct($setVal)
  {
    $this->pri_value = $setVal;
    $this->pub_value = $setVal;
    $this->pro_value = $setVal;
  }
  public function getValue()
  {
    return $this->pri_value;
  }
  public function get_pro_value()
  {
    return $this->pro_value;
  }
};

// array containing objects of MyClass
$input = array (
  array(
  new MyClass(3),
  new MyClass(10),
  ),
  new MyClass(20),
  array(new MyClass(-10))
);

echo "-- For private member --\n";
var_dump( array_walk_recursive($input, "callback_private", 1));
echo "-- For public member --\n";
var_dump( array_walk_recursive($input, "callback_public"));
echo "-- For protected member --\n";
var_dump( array_walk_recursive($input, "callback_protected"));

echo "Done"
?>
--EXPECT--
*** Testing array_walk_recursive() : array of objects ***
-- For private member --
value : int(3)
key : int(0)
value : int(10)
key : int(1)
value : int(20)
key : int(1)
value : int(-10)
key : int(0)
bool(true)
-- For public member --
value : int(3)
value : int(10)
value : int(20)
value : int(-10)
bool(true)
-- For protected member --
value : int(3)
value : int(10)
value : int(20)
value : int(-10)
bool(true)
Done
