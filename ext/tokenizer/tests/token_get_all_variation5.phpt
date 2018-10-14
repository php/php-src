--TEST--
Test token_get_all() function : usage variations - with assignment operators
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Passing 'source' argument with different assignment operators to test them for tokens
 *  += - T_PLUS_EQUAL(277), -= - T_MINUS-EQUAL(276),
 *  *= - T_MUL_EQUAL(275), /= - T_DIVIDE_EQUAL(274),
 *  %= - T_MOD_EQUAL(272), &= - T_AND_EQUAL(271),
 *  |= - T_OR_EQUAL(271), ^= - T_EXOR_EQUAL(269),
 *  >>= - T_SR_EQUAL(267), <<= - T_SL_EQUAL(268), .= - T_CONCAT_EQUAL(273)
*/

echo "*** Testing token_get_all() : 'source' string with different assignment operators ***\n";

// assignment operators : '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '>>=', '<<=', '.='
$source = '<?php 
$a = 1, $b = 2;
$c += $b;
$b -= $a;
$a *= 2;
$d /= 10.50;
$a %= 10.50;
$b &= $c;
$c |= 1;
$d ^= 5;
$a >>= 1;
$b <<= 2;
$d .= "hello world";
?>';
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : 'source' string with different assignment operators ***
array(94) {
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
    string(1) "2"
    [2]=>
    int(2)
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
    int(2)
  }
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(%d)
  }
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
    string(2) "+="
    [2]=>
    int(%d)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
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
  string(1) ";"
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(%d)
  }
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(4)
  }
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "-="
    [2]=>
    int(4)
  }
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(4)
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
    int(4)
  }
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(5)
  }
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "*="
    [2]=>
    int(5)
  }
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(5)
  }
  [35]=>
  string(1) ";"
  [36]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$d"
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
    string(2) "/="
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
    string(5) "10.50"
    [2]=>
    int(6)
  }
  [42]=>
  string(1) ";"
  [43]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(6)
  }
  [44]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(7)
  }
  [45]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "%="
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
    string(5) "10.50"
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
    string(2) "$b"
    [2]=>
    int(8)
  }
  [52]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [53]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "&="
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
    string(2) "$c"
    [2]=>
    int(8)
  }
  [56]=>
  string(1) ";"
  [57]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(8)
  }
  [58]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(9)
  }
  [59]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [60]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "|="
    [2]=>
    int(9)
  }
  [61]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [62]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(9)
  }
  [63]=>
  string(1) ";"
  [64]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(9)
  }
  [65]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$d"
    [2]=>
    int(10)
  }
  [66]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [67]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "^="
    [2]=>
    int(10)
  }
  [68]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [69]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "5"
    [2]=>
    int(10)
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
    int(10)
  }
  [72]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(11)
  }
  [73]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(11)
  }
  [74]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) ">>="
    [2]=>
    int(11)
  }
  [75]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(11)
  }
  [76]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(11)
  }
  [77]=>
  string(1) ";"
  [78]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(11)
  }
  [79]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(12)
  }
  [80]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(12)
  }
  [81]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "<<="
    [2]=>
    int(12)
  }
  [82]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(12)
  }
  [83]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(12)
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
    int(12)
  }
  [86]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$d"
    [2]=>
    int(13)
  }
  [87]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(13)
  }
  [88]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) ".="
    [2]=>
    int(13)
  }
  [89]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(13)
  }
  [90]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(13) ""hello world""
    [2]=>
    int(13)
  }
  [91]=>
  string(1) ";"
  [92]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(13)
  }
  [93]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(14)
  }
}
Done
