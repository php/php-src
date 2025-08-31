--TEST--
Test token_get_all() function : usage variations - with different types of comments
--EXTENSIONS--
tokenizer
--FILE--
<?php
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
array(52) {
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
    string(12) "// int value"
    [2]=>
    int(6)
  }
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(6)
  }
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(7)
  }
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [7]=>
  string(1) "="
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "10"
    [2]=>
    int(7)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(7)
  }
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(8)
  }
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [14]=>
  string(1) "="
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "20"
    [2]=>
    int(8)
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
    int(8)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(9)
  }
  [20]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [21]=>
  string(1) "="
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "true"
    [2]=>
    int(9)
  }
  [24]=>
  string(1) ";"
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "// bool value"
    [2]=>
    int(9)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "

"
    [2]=>
    int(9)
  }
  [28]=>
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
  [29]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(14)
  }
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(15)
  }
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [32]=>
  string(1) "="
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(15)
  }
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [36]=>
  string(1) "+"
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [38]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(15)
  }
  [39]=>
  string(1) ";"
  [40]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(15)
  }
  [41]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "var_dump"
    [2]=>
    int(16)
  }
  [42]=>
  string(1) "("
  [43]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(16)
  }
  [44]=>
  string(1) ")"
  [45]=>
  string(1) ";"
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [47]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(19) "# expected: int(30)"
    [2]=>
    int(16)
  }
  [48]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "

"
    [2]=>
    int(16)
  }
  [49]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(16) "# end of program"
    [2]=>
    int(18)
  }
  [50]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(18)
  }
  [51]=>
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
