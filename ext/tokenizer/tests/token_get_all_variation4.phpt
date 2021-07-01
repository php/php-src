--TEST--
Test token_get_all() function : usage variations - with comparison operators
--EXTENSIONS--
tokenizer
--FILE--
<?php
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
array(88) {
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
    string(2) "if"
    [2]=>
    int(2)
  }
  [2]=>
  string(1) "("
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(2)
  }
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
    string(2) "=="
    [2]=>
    int(2)
  }
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "0"
    [2]=>
    int(2)
  }
  [8]=>
  string(1) ")"
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(2)
  }
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(%d)
  }
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) ""== 0""
    [2]=>
    int(%d)
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
    int(%d)
  }
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "elseif"
    [2]=>
    int(4)
  }
  [16]=>
  string(1) "("
  [17]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(4)
  }
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "==="
    [2]=>
    int(4)
  }
  [20]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [21]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(4)
  }
  [22]=>
  string(1) ")"
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(4)
  }
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(5)
  }
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) ""=== 2""
    [2]=>
    int(5)
  }
  [27]=>
  string(1) ";"
  [28]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [29]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "elseif"
    [2]=>
    int(6)
  }
  [30]=>
  string(1) "("
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(6)
  }
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) ">="
    [2]=>
    int(6)
  }
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "10"
    [2]=>
    int(6)
  }
  [36]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "&&"
    [2]=>
    int(6)
  }
  [38]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [39]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(6)
  }
  [40]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [41]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "<="
    [2]=>
    int(6)
  }
  [42]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [43]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "20"
    [2]=>
    int(6)
  }
  [44]=>
  string(1) ")"
  [45]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(6)
  }
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(7)
  }
  [47]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [48]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(14) "">= 10 & <=20""
    [2]=>
    int(7)
  }
  [49]=>
  string(1) ";"
  [50]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(7)
  }
  [51]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "elseif"
    [2]=>
    int(8)
  }
  [52]=>
  string(1) "("
  [53]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(8)
  }
  [54]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [55]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "!="
    [2]=>
    int(8)
  }
  [56]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [57]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(8)
  }
  [58]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [59]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "||"
    [2]=>
    int(8)
  }
  [60]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [61]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(8)
  }
  [62]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [63]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "<>"
    [2]=>
    int(8)
  }
  [64]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [65]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(8)
  }
  [66]=>
  string(1) ")"
  [67]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(8)
  }
  [68]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(9)
  }
  [69]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [70]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) ""!= 1""
    [2]=>
    int(9)
  }
  [71]=>
  string(1) ";"
  [72]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(9)
  }
  [73]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "elseif"
    [2]=>
    int(10)
  }
  [74]=>
  string(1) "("
  [75]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(10)
  }
  [76]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [77]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "!=="
    [2]=>
    int(10)
  }
  [78]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [79]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(10)
  }
  [80]=>
  string(1) ")"
  [81]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(10)
  }
  [82]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(11)
  }
  [83]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(11)
  }
  [84]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) ""!==1""
    [2]=>
    int(11)
  }
  [85]=>
  string(1) ";"
  [86]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(11)
  }
  [87]=>
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
