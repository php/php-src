--TEST--
Test token_get_all() function : usage variations - with comparison operators
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Passing 'source' argument with different comparison operators to test them for tokens
 *  == - T_IS_EQUAL(283), === - T_IS_IDENTICAL(281), 
 *  >= - T_IS_GREATER_OR_EQUAL(284), <= - T_IS_LESS_OR_EQUAL(285)
 *  != - T_IS_NOT_EQUAL, <> - T_IS_NOT_EQUAL(282), !== - T_IS_NOT_IDENTICAL(280)
*/

echo "*** Testing token_get_all() : 'source' string with different comparison operators ***\n";

// comparison operators : '==', '===', '>=', '<=', '!=', '!==', '<>'
$source = '<?php 
if($a == 0) 
  echo "== 0";
elseif($a === 2)
  echo "=== 2";
elseif($a >= 10 && $a <= 20)
  echo ">= 10 & <=20";
elseif($a != 1 || $a <> 1)
  echo "!= 1";
elseif($a !== 1)
  echo "!==1";
?>';
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : 'source' string with different comparison operators ***
array(89) {
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
    string(2) "if"
    [2]=>
    int(2)
  }
  [3]=>
  string(1) "("
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(2)
  }
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
    string(2) "=="
    [2]=>
    int(2)
  }
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
    string(1) "0"
    [2]=>
    int(2)
  }
  [9]=>
  string(1) ")"
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) " 
  "
    [2]=>
    int(2)
  }
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(%d)
  }
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
    string(6) ""== 0""
    [2]=>
    int(%d)
  }
  [14]=>
  string(1) ";"
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(%d)
  }
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "elseif"
    [2]=>
    int(4)
  }
  [17]=>
  string(1) "("
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(4)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [20]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "==="
    [2]=>
    int(4)
  }
  [21]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(4)
  }
  [23]=>
  string(1) ")"
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(4)
  }
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(5)
  }
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) ""=== 2""
    [2]=>
    int(5)
  }
  [28]=>
  string(1) ";"
  [29]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "elseif"
    [2]=>
    int(6)
  }
  [31]=>
  string(1) "("
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(6)
  }
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) ">="
    [2]=>
    int(6)
  }
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [36]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "10"
    [2]=>
    int(6)
  }
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [38]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "&&"
    [2]=>
    int(6)
  }
  [39]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [40]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(6)
  }
  [41]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [42]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "<="
    [2]=>
    int(6)
  }
  [43]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [44]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "20"
    [2]=>
    int(6)
  }
  [45]=>
  string(1) ")"
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(6)
  }
  [47]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(7)
  }
  [48]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [49]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(14) "">= 10 & <=20""
    [2]=>
    int(7)
  }
  [50]=>
  string(1) ";"
  [51]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(7)
  }
  [52]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "elseif"
    [2]=>
    int(8)
  }
  [53]=>
  string(1) "("
  [54]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(8)
  }
  [55]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [56]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "!="
    [2]=>
    int(8)
  }
  [57]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [58]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(8)
  }
  [59]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [60]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "||"
    [2]=>
    int(8)
  }
  [61]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [62]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(8)
  }
  [63]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [64]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "<>"
    [2]=>
    int(8)
  }
  [65]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [66]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(8)
  }
  [67]=>
  string(1) ")"
  [68]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(8)
  }
  [69]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(9)
  }
  [70]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [71]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) ""!= 1""
    [2]=>
    int(9)
  }
  [72]=>
  string(1) ";"
  [73]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(9)
  }
  [74]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "elseif"
    [2]=>
    int(10)
  }
  [75]=>
  string(1) "("
  [76]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(10)
  }
  [77]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [78]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "!=="
    [2]=>
    int(10)
  }
  [79]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [80]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(10)
  }
  [81]=>
  string(1) ")"
  [82]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(10)
  }
  [83]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(11)
  }
  [84]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(11)
  }
  [85]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) ""!==1""
    [2]=>
    int(11)
  }
  [86]=>
  string(1) ";"
  [87]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(11)
  }
  [88]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(12)
  }
}
Done
