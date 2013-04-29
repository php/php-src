--TEST--
Test token_get_all() function : usage variations - with predefined language constants
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP language tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Testing token_get_all() with following predefined language constants:
 *   __FILE__     - T_FILE
 *   __CLASS__    - T_CLASS_C
 *   __TRAIT__    - T_TRAIT_C
 *   __FUNCTION__ - T_FUNC_C
 *   __LINE__     - T_LINE
 *   __METHOD__   - T_METHOD_C
*/

echo "*** Testing token_get_all() : with language constants ***\n";

// parsing __FILE__ token
echo "-- with FILE --\n";
$source = "<?php 
\$fp =  fopen(__FILE__, 'r');
?>";
var_dump( token_get_all($source));

// parsing __CLASS__, __TRAIT__ and __FUNCTION__ tokens
echo "-- with CLASS, TRAIT and FUNCTION --\n";
$source = '<?php
class MyClass
{
  echo  __CLASS__;
  echo  __TRAIT__;
  function myFunction()
  {  echo  __FUNCTION__; }
}
?>';
var_dump( token_get_all($source));

// parsing __LINE__ and __METHOD__ tokens
echo "-- with LINE and METHOD --\n";
$source = '<?php
  $a = __LINE__;
  $b = $b.__METHOD__;
?>';
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : with language constants ***
-- with FILE --
array(16) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "$fp"
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [4]=>
  string(1) "="
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "  "
    [2]=>
    int(2)
  }
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "fopen"
    [2]=>
    int(2)
  }
  [7]=>
  string(1) "("
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "__FILE__"
    [2]=>
    int(2)
  }
  [9]=>
  string(1) ","
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "'r'"
    [2]=>
    int(2)
  }
  [12]=>
  string(1) ")"
  [13]=>
  string(1) ";"
  [14]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(%d)
  }
}
-- with CLASS, TRAIT and FUNCTION --
array(35) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php
"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "class"
    [2]=>
    int(2)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "MyClass"
    [2]=>
    int(2)
  }
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [5]=>
  string(1) "{"
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(%d)
  }
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(4)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "  "
    [2]=>
    int(4)
  }
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(9) "__CLASS__"
    [2]=>
    int(4)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(%d)
  }
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(5)
  }
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "  "
    [2]=>
    int(5)
  }
  [14]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(9) "__TRAIT__"
    [2]=>
    int(5)
  }
  [15]=>
  string(1) ";"
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(5)
  }
  [17]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "function"
    [2]=>
    int(6)
  }
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(10) "myFunction"
    [2]=>
    int(6)
  }
  [20]=>
  string(1) "("
  [21]=>
  string(1) ")"
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(6)
  }
  [23]=>
  string(1) "{"
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "  "
    [2]=>
    int(7)
  }
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(7)
  }
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "  "
    [2]=>
    int(7)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(12) "__FUNCTION__"
    [2]=>
    int(7)
  }
  [28]=>
  string(1) ";"
  [29]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [30]=>
  string(1) "}"
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(7)
  }
  [32]=>
  string(1) "}"
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(8)
  }
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(9)
  }
}
-- with LINE and METHOD --
array(19) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php
"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "  "
    [2]=>
    int(2)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [4]=>
  string(1) "="
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "__LINE__"
    [2]=>
    int(2)
  }
  [7]=>
  string(1) ";"
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(2)
  }
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(%d)
  }
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [11]=>
  string(1) "="
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(%d)
  }
  [14]=>
  string(1) "."
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(10) "__METHOD__"
    [2]=>
    int(%d)
  }
  [16]=>
  string(1) ";"
  [17]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(%d)
  }
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(4)
  }
}
Done
