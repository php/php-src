--TEST--
Test token_get_all() function : usage variations - with predefined language constants
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Testing token_get_all() with following predefined language constants:
 *   __FILE__ - T_FILE(364)
 *   __CLASS__ - T_CLASS_C(360)
 *   __FUNCTION__ - T_FUNC_C(362)
 *   __LINE__ - T_LINE(363)
 *   __METHOD__ - T_METHOD_C(361)
*/

echo "*** Testing token_get_all() : with language constants ***\n";

// parsing __FILE__ token
echo "-- with FILE --\n";
$source = "<?php 
\$fp =  fopen(__FILE__, 'r');
?>";
var_dump( token_get_all($source));

// parsing __CLASS__ and __FUNCTION__ tokens
echo "-- with CLASS and FUNCTION --\n";
$source = '<?php
class MyClass
{
  echo  __CLASS__;
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
    int(367)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(3) "$fp"
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(370)
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
    int(370)
    [1]=>
    string(2) "  "
    [2]=>
    int(2)
  }
  [6]=>
  array(3) {
    [0]=>
    int(307)
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
    int(364)
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
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [11]=>
  array(3) {
    [0]=>
    int(315)
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
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [15]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(3)
  }
}
-- with CLASS and FUNCTION --
array(30) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
    [1]=>
    string(6) "<?php
"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(352)
    [1]=>
    string(5) "class"
    [2]=>
    int(2)
  }
  [2]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(7) "MyClass"
    [2]=>
    int(2)
  }
  [4]=>
  array(3) {
    [0]=>
    int(370)
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
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(3)
  }
  [7]=>
  array(3) {
    [0]=>
    int(316)
    [1]=>
    string(4) "echo"
    [2]=>
    int(4)
  }
  [8]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "  "
    [2]=>
    int(4)
  }
  [9]=>
  array(3) {
    [0]=>
    int(360)
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
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(4)
  }
  [12]=>
  array(3) {
    [0]=>
    int(333)
    [1]=>
    string(8) "function"
    [2]=>
    int(5)
  }
  [13]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [14]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "myFunction"
    [2]=>
    int(5)
  }
  [15]=>
  string(1) "("
  [16]=>
  string(1) ")"
  [17]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(5)
  }
  [18]=>
  string(1) "{"
  [19]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "  "
    [2]=>
    int(6)
  }
  [20]=>
  array(3) {
    [0]=>
    int(316)
    [1]=>
    string(4) "echo"
    [2]=>
    int(6)
  }
  [21]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "  "
    [2]=>
    int(6)
  }
  [22]=>
  array(3) {
    [0]=>
    int(362)
    [1]=>
    string(12) "__FUNCTION__"
    [2]=>
    int(6)
  }
  [23]=>
  string(1) ";"
  [24]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [25]=>
  string(1) "}"
  [26]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(6)
  }
  [27]=>
  string(1) "}"
  [28]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(7)
  }
  [29]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(8)
  }
}
-- with LINE and METHOD --
array(19) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
    [1]=>
    string(6) "<?php
"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "  "
    [2]=>
    int(2)
  }
  [2]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(370)
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
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [6]=>
  array(3) {
    [0]=>
    int(363)
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
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(2)
  }
  [9]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(3)
  }
  [10]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(3)
  }
  [11]=>
  string(1) "="
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(3)
  }
  [13]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(3)
  }
  [14]=>
  string(1) "."
  [15]=>
  array(3) {
    [0]=>
    int(361)
    [1]=>
    string(10) "__METHOD__"
    [2]=>
    int(3)
  }
  [16]=>
  string(1) ";"
  [17]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(3)
  }
  [18]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(4)
  }
}
Done
