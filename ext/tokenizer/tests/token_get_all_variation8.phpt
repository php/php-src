--TEST--
Test token_get_all() function : usage variations - with type casting operators
--EXTENSIONS--
tokenizer
--FILE--
<?php
/*
 * Passing 'source' argument with different type casting operators to test them for token
 *  (int)/(integer) - T_INT_CAST(295), (float)/(real)/(double) - T_DOUBLE_CAST(294),
 *  (string) - T_STRING_CAST(293), (bool)/(boolean) - T_BOOL_CAST(290),
 *  (unset) - T_UNSET_CAST(289)
*/

echo "*** Testing token_get_all() : 'source' string with different type casting operators ***\n";

// type casting operators : (int), (integer), (float), (double), (string), (array), (object), (bool), (boolean),(unset)
$source = '<?php
$a = 1, $b = 10.5
$c = (int)$b + $a;
$d = (float)$a + $b;
$e = (string)$a.(string)$b;
if((bool)$a) echo "true";
if(!(boolean)$b) echo "false";
$c = $c + (integer) 123.4e2;
$d = $c - (real) 12;
$b = (unset)$a;
?>';
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : 'source' string with different type casting operators ***
array(107) {
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
    string(1) "1"
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
    string(4) "10.5"
    [2]=>
    int(2)
  }
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [14]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(%d)
  }
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [16]=>
  string(1) "="
  [17]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "(int)"
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
    int(%d)
  }
  [20]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [21]=>
  string(1) "+"
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(%d)
  }
  [24]=>
  string(1) ";"
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(%d)
  }
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$d"
    [2]=>
    int(4)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [28]=>
  string(1) "="
  [29]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "(float)"
    [2]=>
    int(4)
  }
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(4)
  }
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [33]=>
  string(1) "+"
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(4)
  }
  [36]=>
  string(1) ";"
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [38]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$e"
    [2]=>
    int(5)
  }
  [39]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [40]=>
  string(1) "="
  [41]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [42]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "(string)"
    [2]=>
    int(5)
  }
  [43]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(5)
  }
  [44]=>
  string(1) "."
  [45]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "(string)"
    [2]=>
    int(5)
  }
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(5)
  }
  [47]=>
  string(1) ";"
  [48]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [49]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
    [2]=>
    int(6)
  }
  [50]=>
  string(1) "("
  [51]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "(bool)"
    [2]=>
    int(6)
  }
  [52]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(6)
  }
  [53]=>
  string(1) ")"
  [54]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [55]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(6)
  }
  [56]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [57]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) ""true""
    [2]=>
    int(6)
  }
  [58]=>
  string(1) ";"
  [59]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(6)
  }
  [60]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
    [2]=>
    int(7)
  }
  [61]=>
  string(1) "("
  [62]=>
  string(1) "!"
  [63]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(9) "(boolean)"
    [2]=>
    int(7)
  }
  [64]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(7)
  }
  [65]=>
  string(1) ")"
  [66]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [67]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(7)
  }
  [68]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [69]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) ""false""
    [2]=>
    int(7)
  }
  [70]=>
  string(1) ";"
  [71]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(7)
  }
  [72]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(8)
  }
  [73]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [74]=>
  string(1) "="
  [75]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [76]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(8)
  }
  [77]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [78]=>
  string(1) "+"
  [79]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [80]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(9) "(integer)"
    [2]=>
    int(8)
  }
  [81]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [82]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "123.4e2"
    [2]=>
    int(8)
  }
  [83]=>
  string(1) ";"
  [84]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(8)
  }
  [85]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$d"
    [2]=>
    int(9)
  }
  [86]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [87]=>
  string(1) "="
  [88]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [89]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(9)
  }
  [90]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [91]=>
  string(1) "-"
  [92]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [93]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "(real)"
    [2]=>
    int(9)
  }
  [94]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [95]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "12"
    [2]=>
    int(9)
  }
  [96]=>
  string(1) ";"
  [97]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(9)
  }
  [98]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(10)
  }
  [99]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [100]=>
  string(1) "="
  [101]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [102]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "(unset)"
    [2]=>
    int(10)
  }
  [103]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(10)
  }
  [104]=>
  string(1) ";"
  [105]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(10)
  }
  [106]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(11)
  }
}
Done
