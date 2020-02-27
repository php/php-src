--TEST--
Test token_get_all() function : usage variations - with different types of comments
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP language tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Passing 'source' argument with different style of comments
 */
//  '//', '/* */', '#' - T_COMMENT(365)
// '/** */' - T_DOC_COMMENT(366)


echo "*** Testing token_get_all() : 'source' string with different comments ***\n";

// types of comments: '//', '/* */', '#' & /** */

$source = '<?php
/** Performing addition operation on given values :
  * a, b
  */

// int value
$a = 10;
$b = 20;
$c = true; // bool value

/*
 * Performing operation on $a,$b
 * display result
 */
$c = $a + $b;
var_dump($c); # expected: int(30)

# end of program
?>';
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : 'source' string with different comments ***
array(50) {
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
    string(65) "/** Performing addition operation on given values :
  * a, b
  */"
    [2]=>
    int(2)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "

"
    [2]=>
    int(4)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "// int value
"
    [2]=>
    int(6)
  }
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(7)
  }
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [6]=>
  string(1) "="
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "10"
    [2]=>
    int(7)
  }
  [9]=>
  string(1) ";"
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(7)
  }
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(8)
  }
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [13]=>
  string(1) "="
  [14]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "20"
    [2]=>
    int(8)
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
    int(8)
  }
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(9)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [20]=>
  string(1) "="
  [21]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "true"
    [2]=>
    int(9)
  }
  [23]=>
  string(1) ";"
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(14) "// bool value
"
    [2]=>
    int(9)
  }
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(10)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(57) "/*
 * Performing operation on $a,$b
 * display result
 */"
    [2]=>
    int(11)
  }
  [28]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(14)
  }
  [29]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(15)
  }
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [31]=>
  string(1) "="
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(15)
  }
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [35]=>
  string(1) "+"
  [36]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(15)
  }
  [38]=>
  string(1) ";"
  [39]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(15)
  }
  [40]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "var_dump"
    [2]=>
    int(16)
  }
  [41]=>
  string(1) "("
  [42]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(16)
  }
  [43]=>
  string(1) ")"
  [44]=>
  string(1) ";"
  [45]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(20) "# expected: int(%d)
"
    [2]=>
    int(16)
  }
  [47]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(17)
  }
  [48]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(17) "# end of program
"
    [2]=>
    int(18)
  }
  [49]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(19)
  }
}
Done
