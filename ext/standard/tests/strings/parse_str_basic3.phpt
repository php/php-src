--TEST--
Test parse_str() function : basic functionality
--INI--
magic_quotes_gpc = on
--FILE--
<?php
/* Prototype  : void parse_str  ( string $str  [, array &$arr  ] )
 * Description: Parses the string into variables
 * Source code: ext/standard/string.c
*/

echo "*** Testing parse_str() : basic functionality ***\n";

echo "\nTest string with array values\n";
$s1 = "first=abc&a[]=123&a[]=false&b[]=str&c[]=3.5&a[]=last";
var_dump(parse_str($s1));
var_dump($first, $a, $b, $c); 

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

echo "\nTest string with array values with same name as existing variable\n";
$a = 9999;
$s1 = "a[]=123&a[]=false&a[]=last";
var_dump(parse_str($s1));
var_dump($a);

echo "\nTest string with non-array value with same name as existing array variable\n";
$a = array(10,11,12,13);
$s1 = "a=999";
parse_str($s1);
var_dump($a);

echo "\nTest string with encoded data\n";
$s1 = "a=%3c%3d%3d%20%20foo+bar++%3d%3d%3e&b=%23%23%23Hello+World%23%23%23";
parse_str($s1);
var_dump($a, $b);

echo "\nTest string with single quotes characters\n";
$s1 = "firstname=Bill&surname=O%27Reilly";
var_dump(parse_str($s1));
var_dump($firstname, $surname);

echo "\nTest string with backslash characters\n";
$s1 = "sum=10%5c2%3d5";
var_dump(parse_str($s1));
var_dump($sum);

echo "\nTest string with double quotes data\n";
$s1 = "str=A+string+with+%22quoted%22+strings";
var_dump(parse_str($s1));
var_dump($str);

echo "\nTest string with nulls\n";
$s1 = "str=A%20string%20with%20containing%20%00%00%00%20nulls";
var_dump(parse_str($s1));
var_dump($str);

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
===DONE===
--EXPECTF--
Warning: Directive 'magic_quotes_gpc' is deprecated in PHP 5.3 and greater in Unknown on line 0
*** Testing parse_str() : basic functionality ***

Test string with array values
NULL
string(3) "abc"
array(3) {
  [0]=>
  string(3) "123"
  [1]=>
  string(5) "false"
  [2]=>
  string(4) "last"
}
array(1) {
  [0]=>
  string(3) "str"
}
array(1) {
  [0]=>
  string(3) "3.5"
}

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

Test string with array values with same name as existing variable
NULL
array(3) {
  [0]=>
  string(3) "123"
  [1]=>
  string(5) "false"
  [2]=>
  string(4) "last"
}

Test string with non-array value with same name as existing array variable
string(3) "999"

Test string with encoded data
string(17) "<==  foo bar  ==>"
string(17) "###Hello World###"

Test string with single quotes characters
NULL
string(4) "Bill"
string(9) "O\'Reilly"

Test string with backslash characters
NULL
string(7) "10\\2=5"

Test string with double quotes data
NULL
string(32) "A string with \"quoted\" strings"

Test string with nulls
NULL
string(37) "A string with containing \0\0\0 nulls"

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
===DONE===
