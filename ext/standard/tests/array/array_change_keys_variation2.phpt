--TEST--
Test array_change_keys() function with invalid callback results
--FILE--
<?php

$oldArray = range(1, 5);

echo "--- Test with function that doesn't return ---\n";
var_dump(array_change_keys($oldArray, function(){}));

echo "--- Test with function that always returns null ---\n";
var_dump(array_change_keys($oldArray, function(){
    return null;
}));

echo "--- Test with function that doesn't always return ---\n";
var_dump(array_change_keys($oldArray, function($k){
    if ($k % 2) {
        return $k;
    }
}));

echo "--- Test with function that returns streams ---\n";
var_dump(array_change_keys($oldArray, function(){
    return fopen('php://memory','r+');
}));

echo "--- Test with function that returns objects that don't implement __toString() ---\n";
var_dump(array_change_keys($oldArray, function(){
    return new \DateTime();
}));

echo "--- Test with function that returns objects that do implement __toString(), which is unsupported ---\n";
class FancyString
{
    private $str;

    public function __construct($str)
    {
        $this->str = $str;
    }

    public function __toString()
    {
        return $this->str;
    }
}

var_dump(array_change_keys(range(1, 5), function($k, $v) {
    return new FancyString($v);
}));

echo "--- Test with invalid callable references ---\n";
var_dump(array_change_keys($oldArray, null));
var_dump(array_change_keys($oldArray, 'some_function_that_doesnt_exist'));
var_dump(array_change_keys($oldArray, ['foo', 'bar']));

?>
--EXPECTF--
--- Test with function that doesn't return ---

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d
array(0) {
}
--- Test with function that always returns null ---

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d
array(0) {
}
--- Test with function that doesn't always return ---

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d
array(2) {
  [1]=>
  int(2)
  [3]=>
  int(4)
}
--- Test with function that returns streams ---

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d
array(0) {
}
--- Test with function that returns objects that don't implement __toString() ---

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d
array(0) {
}
--- Test with function that returns objects that do implement __toString(), which is unsupported ---

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d

Warning: array_change_keys(): New key should be either a string or an integer in %s on line %d
array(0) {
}
--- Test with invalid callable references ---

Warning: array_change_keys() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
NULL
Warning: array_change_keys() expects parameter 2 to be a valid callback, function 'some_function_that_doesnt_exist' not found or invalid function name in %s on line %d
NULL
Warning: array_change_keys() expects parameter 2 to be a valid callback, class 'foo' not found in %s on line %d
NULL