--TEST--
Test parse_str() function : basic functionality
--INI--
max_input_vars=100
filter.default=unsafe_raw
--FILE--
<?php
echo "*** Testing parse_str() : basic functionality ***\n";

echo "\nTest string with array values and results array\n";
$s1 = "first=abc&a[]=123&a[]=false&b[]=str&c[]=3.5&a[]=last";
var_dump(parse_str($s1, $res3_array));
var_dump($res3_array);

echo "\nTest string containing numerical array keys\n";
$str = "arr[1]=sid&arr[4]=bill";
var_dump(parse_str($str, $res));
var_dump($res);

echo "\nTest string containing associative keys\n";
$str = "arr[first]=sid&arr[forth]=bill";
var_dump(parse_str($str, $res));
var_dump($res);

echo "\nTest string with encoded data\n";
$s1 = "a=%3c%3d%3d%20%20foo+bar++%3d%3d%3e&b=%23%23%23Hello+World%23%23%23";
parse_str($s1, $res);
var_dump($res);

echo "\nTest string with single quotes characters\n";
$s1 = "firstname=Bill&surname=O%27Reilly";
var_dump(parse_str($s1, $res));
var_dump($res);

echo "\nTest string with backslash characters\n";
$s1 = "sum=10%5c2%3d5";
var_dump(parse_str($s1, $res));
var_dump($res);

echo "\nTest string with double quotes data\n";
$s1 = "str=A+string+with+%22quoted%22+strings";
var_dump(parse_str($s1, $res));
var_dump($res);

echo "\nTest string with nulls\n";
$s1 = "str=A%20string%20with%20containing%20%00%00%00%20nulls";
var_dump(parse_str($s1, $res));
var_dump($res);

echo "\nTest string with 2-dim array with numeric keys\n";
$str = "arr[3][4]=sid&arr[3][6]=fred";
var_dump(parse_str($str, $res));
var_dump($res);

echo "\nTest string with 2-dim array with null keys\n";
$str = "arr[][]=sid&arr[][]=fred";
var_dump(parse_str($str, $res));
var_dump($res);

echo "\nTest string with 2-dim array with non-numeric keys\n";
$str = "arr[one][four]=sid&arr[three][six]=fred";
var_dump(parse_str($str, $res));
var_dump($res);

echo "\nTest string with 3-dim array with numeric keys\n";
$str = "arr[1][2][3]=sid&arr[1][2][6]=fred";
var_dump(parse_str($str, $res));
var_dump($res);

?>
--EXPECTF--
*** Testing parse_str() : basic functionality ***

Test string with array values and results array
NULL
array(4) {
  ["first"]=>
  string(3) "abc"
  ["a"]=>
  array(3) {
    [0]=>
    string(3) "123"
    [1]=>
    string(5) "false"
    [2]=>
    string(4) "last"
  }
  ["b"]=>
  array(1) {
    [0]=>
    string(3) "str"
  }
  ["c"]=>
  array(1) {
    [0]=>
    string(3) "3.5"
  }
}

Test string containing numerical array keys
NULL
array(1) {
  ["arr"]=>
  array(2) {
    [1]=>
    string(3) "sid"
    [4]=>
    string(4) "bill"
  }
}

Test string containing associative keys
NULL
array(1) {
  ["arr"]=>
  array(2) {
    ["first"]=>
    string(3) "sid"
    ["forth"]=>
    string(4) "bill"
  }
}

Test string with encoded data
array(2) {
  ["a"]=>
  string(17) "<==  foo bar  ==>"
  ["b"]=>
  string(17) "###Hello World###"
}

Test string with single quotes characters
NULL
array(2) {
  ["firstname"]=>
  string(4) "Bill"
  ["surname"]=>
  string(8) "O'Reilly"
}

Test string with backslash characters
NULL
array(1) {
  ["sum"]=>
  string(6) "10\2=5"
}

Test string with double quotes data
NULL
array(1) {
  ["str"]=>
  string(30) "A string with "quoted" strings"
}

Test string with nulls
NULL
array(1) {
  ["str"]=>
  string(34) "A string with containing %0%0%0 nulls"
}

Test string with 2-dim array with numeric keys
NULL
array(1) {
  ["arr"]=>
  array(1) {
    [3]=>
    array(2) {
      [4]=>
      string(3) "sid"
      [6]=>
      string(4) "fred"
    }
  }
}

Test string with 2-dim array with null keys
NULL
array(1) {
  ["arr"]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(3) "sid"
    }
    [1]=>
    array(1) {
      [0]=>
      string(4) "fred"
    }
  }
}

Test string with 2-dim array with non-numeric keys
NULL
array(1) {
  ["arr"]=>
  array(2) {
    ["one"]=>
    array(1) {
      ["four"]=>
      string(3) "sid"
    }
    ["three"]=>
    array(1) {
      ["six"]=>
      string(4) "fred"
    }
  }
}

Test string with 3-dim array with numeric keys
NULL
array(1) {
  ["arr"]=>
  array(1) {
    [1]=>
    array(1) {
      [2]=>
      array(2) {
        [3]=>
        string(3) "sid"
        [6]=>
        string(4) "fred"
      }
    }
  }
}
