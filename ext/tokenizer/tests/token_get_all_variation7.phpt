--TEST--
Test token_get_all() function : usage variations - with increment/decrement operators
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Passing 'source' argument with different increment/decrement operators to test them for token
 *  ++ - T_INC(297)
 *  -- - T_DEC(296)
*/

echo "*** Testing token_get_all() : 'source' string with different increment/decrement operators ***\n";

// increment/decrement operators : '++' , '--'
$source = '<?php 
$a = 10, $b = 5;
$a++;
$b--;
echo $a;
?>';
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : 'source' string with different increment/decrement operators ***
array(30) {
  [0]=>
  array(3) {
    [0]=>
    int(368)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(371)
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
    string(2) "$a"
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(371)
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
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [6]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(2) "10"
    [2]=>
    int(2)
  }
  [7]=>
  string(1) ","
  [8]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
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
    int(2)
  }
  [10]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [11]=>
  string(1) "="
  [12]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [13]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "5"
    [2]=>
    int(2)
  }
  [14]=>
  string(1) ";"
  [15]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [16]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(3)
  }
  [17]=>
  array(3) {
    [0]=>
    int(297)
    [1]=>
    string(2) "++"
    [2]=>
    int(3)
  }
  [18]=>
  string(1) ";"
  [19]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) "
"
    [2]=>
    int(3)
  }
  [20]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(4)
  }
  [21]=>
  array(3) {
    [0]=>
    int(296)
    [1]=>
    string(2) "--"
    [2]=>
    int(4)
  }
  [22]=>
  string(1) ";"
  [23]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [24]=>
  array(3) {
    [0]=>
    int(316)
    [1]=>
    string(4) "echo"
    [2]=>
    int(5)
  }
  [25]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [26]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(5)
  }
  [27]=>
  string(1) ";"
  [28]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [29]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "?>"
    [2]=>
    int(6)
  }
}
Done
