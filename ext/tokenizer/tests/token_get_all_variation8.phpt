--TEST--
Test token_get_all() function : usage variations - with type casting operators
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Passing 'source' argument with different type casting operators to test them for token
 *  (int)/(integer) - T_INT_CAST(295), (float)/(real)/(double) - T_DOUBLE_CAST(294),
 *  (string) - T_STRING_CAST(293), (bool)/(boolean) - T_BOOL_CAST(290),
 *  (unset) - T_UNSET_CAST(289)
*/

echo "*** Testing token_get_all() : 'source' string with different type casting operators ***\n";

// type casting operators : (int), (integer), (float), (real), (double), (string), (array), (object), (bool), (boolean),(unset)
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
array(108) {
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
    string(1) "1"
    [2]=>
    int(2)
  }
  [7]=>
  string(1) ","
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
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
    int(2)
  }
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
  string(1) "="
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "10.5"
    [2]=>
    int(2)
  }
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
    string(2) "$c"
    [2]=>
    int(%d)
  }
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [17]=>
  string(1) "="
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "(int)"
    [2]=>
    int(%d)
  }
  [20]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(%d)
  }
  [21]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [22]=>
  string(1) "+"
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(%d)
  }
  [25]=>
  string(1) ";"
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(%d)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$d"
    [2]=>
    int(4)
  }
  [28]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [29]=>
  string(1) "="
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "(float)"
    [2]=>
    int(4)
  }
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(4)
  }
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [34]=>
  string(1) "+"
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [36]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(4)
  }
  [37]=>
  string(1) ";"
  [38]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [39]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$e"
    [2]=>
    int(5)
  }
  [40]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [41]=>
  string(1) "="
  [42]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [43]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "(string)"
    [2]=>
    int(5)
  }
  [44]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(5)
  }
  [45]=>
  string(1) "."
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "(string)"
    [2]=>
    int(5)
  }
  [47]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(5)
  }
  [48]=>
  string(1) ";"
  [49]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [50]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
    [2]=>
    int(6)
  }
  [51]=>
  string(1) "("
  [52]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "(bool)"
    [2]=>
    int(6)
  }
  [53]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(6)
  }
  [54]=>
  string(1) ")"
  [55]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [56]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(6)
  }
  [57]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [58]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) ""true""
    [2]=>
    int(6)
  }
  [59]=>
  string(1) ";"
  [60]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(6)
  }
  [61]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
    [2]=>
    int(7)
  }
  [62]=>
  string(1) "("
  [63]=>
  string(1) "!"
  [64]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(9) "(boolean)"
    [2]=>
    int(7)
  }
  [65]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(7)
  }
  [66]=>
  string(1) ")"
  [67]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [68]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(7)
  }
  [69]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [70]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) ""false""
    [2]=>
    int(7)
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
    int(7)
  }
  [73]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(8)
  }
  [74]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [75]=>
  string(1) "="
  [76]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [77]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(8)
  }
  [78]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [79]=>
  string(1) "+"
  [80]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [81]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(9) "(integer)"
    [2]=>
    int(8)
  }
  [82]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [83]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "123.4e2"
    [2]=>
    int(8)
  }
  [84]=>
  string(1) ";"
  [85]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(8)
  }
  [86]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$d"
    [2]=>
    int(9)
  }
  [87]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [88]=>
  string(1) "="
  [89]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [90]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(9)
  }
  [91]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [92]=>
  string(1) "-"
  [93]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [94]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "(real)"
    [2]=>
    int(9)
  }
  [95]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [96]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "12"
    [2]=>
    int(9)
  }
  [97]=>
  string(1) ";"
  [98]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(9)
  }
  [99]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(10)
  }
  [100]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [101]=>
  string(1) "="
  [102]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [103]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "(unset)"
    [2]=>
    int(10)
  }
  [104]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(10)
  }
  [105]=>
  string(1) ";"
  [106]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(10)
  }
  [107]=>
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
