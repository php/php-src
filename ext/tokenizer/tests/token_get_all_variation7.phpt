--TEST--
Test token_get_all() function : usage variations - with increment/decrement operators
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
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
var_dump(token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : 'source' string with different increment/decrement operators ***
array(29) {
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
    string(2) "$a"
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
  string(1) "="
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "10"
    [2]=>
    int(2)
  }
  [6]=>
  string(1) ","
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(2)
  }
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [10]=>
  string(1) "="
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "5"
    [2]=>
    int(2)
  }
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
    string(2) "$a"
    [2]=>
    int(%d)
  }
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "++"
    [2]=>
    int(%d)
  }
  [17]=>
  string(1) ";"
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(%d)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(4)
  }
  [20]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "--"
    [2]=>
    int(4)
  }
  [21]=>
  string(1) ";"
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(5)
  }
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(5)
  }
  [26]=>
  string(1) ";"
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [28]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(6)
  }
}
Done
