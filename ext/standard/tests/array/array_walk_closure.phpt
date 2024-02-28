--TEST--
array_walk() closure tests
--FILE--
<?php

$ar = ["one" => 1, "two"=>2, "three" => 3];
var_dump(array_walk($ar, function(){ var_dump(func_get_args());}));

echo "\nclosure with array\n";
$ar = ["one" => 1, "two"=>2, "three" => 3];
$user_data = ["sum" => 42];
$func = function($value, $key, &$udata) {
    var_dump($udata);
    $udata["sum"] += $value;
};

var_dump(array_walk($ar, $func, $user_data));
echo "End result:";
var_dump($user_data["sum"]);

echo "\nclosure with use\n";
$ar = ["one" => 1, "two"=>2, "three" => 3];
$user_data = ["sum" => 42];
$func = function($value, $key) use (&$user_data) {
    var_dump($user_data);
    $user_data["sum"] += $value;
};

var_dump(array_walk($ar, $func, $user_data));
echo "End result:";
var_dump($user_data["sum"]);


echo "\nclosure with object\n";
$ar = ["one" => 1, "two"=>2, "three" => 3];
$user_data = (object)["sum" => 42];
$func = function($value, $key, &$udata) {
    var_dump($udata);
    $udata->sum += $value;
};

var_dump(array_walk($ar, $func, $user_data));
echo "End result:";
var_dump($user_data->sum);



echo "\nfunction with object\n";
function sum_it_up_object($value, $key, $udata)
{
    var_dump($udata);
    $udata->sum += $value;
}

$ar = ["one" => 1, "two"=>2, "three" => 3];
$user_data = (object)["sum" => 42];

var_dump(array_walk($ar, "sum_it_up_object", $user_data));
echo "End result:";
var_dump($user_data->sum);


echo "\nfunction with array\n";
function sum_it_up_array($value, $key, $udata)
{
    var_dump($udata);
    $udata['sum'] += $value;
}

$ar = ["one" => 1, "two"=>2, "three" => 3];
$user_data = ["sum" => 42];

var_dump(array_walk($ar, "sum_it_up_array", $user_data));
echo "End result:";
var_dump($user_data['sum']);

echo "\nclosure and exception\n";
$ar = ["one" => 1, "two"=>2, "three" => 3];
try {
    var_dump(array_walk($ar, function($v, $k) { if ($v == 2) throw new Exception; } ));
} catch (Exception $e) {
    var_dump($e->getTrace());
}


echo "Done\n";
?>
--EXPECTF--
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(3) "one"
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  string(3) "two"
}
array(2) {
  [0]=>
  int(3)
  [1]=>
  string(5) "three"
}
bool(true)

closure with array

Warning: {closure:%s:%d}(): Argument #3 ($udata) must be passed by reference, value given in %s on line %d
array(1) {
  ["sum"]=>
  int(42)
}

Warning: {closure:%s:%d}(): Argument #3 ($udata) must be passed by reference, value given in %s on line %d
array(1) {
  ["sum"]=>
  int(42)
}

Warning: {closure:%s:%d}(): Argument #3 ($udata) must be passed by reference, value given in %s on line %d
array(1) {
  ["sum"]=>
  int(42)
}
bool(true)
End result:int(42)

closure with use
array(1) {
  ["sum"]=>
  int(42)
}
array(1) {
  ["sum"]=>
  int(43)
}
array(1) {
  ["sum"]=>
  int(45)
}
bool(true)
End result:int(48)

closure with object

Warning: {closure:%s:%d}(): Argument #3 ($udata) must be passed by reference, value given in %s on line %d
object(stdClass)#1 (1) {
  ["sum"]=>
  int(42)
}

Warning: {closure:%s:%d}(): Argument #3 ($udata) must be passed by reference, value given in %s on line %d
object(stdClass)#1 (1) {
  ["sum"]=>
  int(43)
}

Warning: {closure:%s:%d}(): Argument #3 ($udata) must be passed by reference, value given in %s on line %d
object(stdClass)#1 (1) {
  ["sum"]=>
  int(45)
}
bool(true)
End result:int(48)

function with object
object(stdClass)#2 (1) {
  ["sum"]=>
  int(42)
}
object(stdClass)#2 (1) {
  ["sum"]=>
  int(43)
}
object(stdClass)#2 (1) {
  ["sum"]=>
  int(45)
}
bool(true)
End result:int(48)

function with array
array(1) {
  ["sum"]=>
  int(42)
}
array(1) {
  ["sum"]=>
  int(42)
}
array(1) {
  ["sum"]=>
  int(42)
}
bool(true)
End result:int(42)

closure and exception
array(2) {
  [0]=>
  array(2) {
    ["function"]=>
    string(%d) "{closure:%s:%d}"
    ["args"]=>
    array(2) {
      [0]=>
      int(2)
      [1]=>
      string(3) "two"
    }
  }
  [1]=>
  array(4) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(10) "array_walk"
    ["args"]=>
    array(2) {
      [0]=>
      array(3) {
        ["one"]=>
        int(1)
        ["two"]=>
        int(2)
        ["three"]=>
        int(3)
      }
      [1]=>
      object(Closure)#%d (2) {
        ["name"]=>
        string(%d) "{closure:%s:%d}"
        ["parameter"]=>
        array(2) {
          ["$v"]=>
          string(10) "<required>"
          ["$k"]=>
          string(10) "<required>"
        }
      }
    }
  }
}
Done
