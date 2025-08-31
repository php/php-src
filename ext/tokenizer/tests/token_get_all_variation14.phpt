--TEST--
Test token_get_all() function : usage variations - invalid token values
--EXTENSIONS--
tokenizer
--FILE--
<?php
/*
 * Testing token_get_all() with 'source' string containing invalid/unknown token value
 *  unknown tokens - T_UNKNOWN(307)
*/

echo "*** Testing token_get_all() : with invalid/unknown tokens ***\n";

// with valid php tags and invalid tokens
echo "-- with valid PHP tags & invlid tokens --\n";
$source = '<?php
struct myStruct {
  variable $a;
  method() { display $a; }
}
?>';
var_dump( token_get_all($source));

// with invalid open tag for testing entire source to be unknown token
echo "-- with invlalid PHP open tag & valid tokens --\n";
$source = '<pli
echo "hello world"; ?>';
var_dump( token_get_all($source));

// with invalid PHP tags and invalid tokens
echo "-- with invalid PHP tags and tokens --\n";
$source = '<PDP display  $a; <';
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : with invalid/unknown tokens ***
-- with valid PHP tags & invlid tokens --
array(28) {
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
    string(6) "struct"
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
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "myStruct"
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
  string(1) "{"
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(2)
  }
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "variable"
    [2]=>
    int(%d)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(%d)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(%d)
  }
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "method"
    [2]=>
    int(4)
  }
  [13]=>
  string(1) "("
  [14]=>
  string(1) ")"
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [16]=>
  string(1) "{"
  [17]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "display"
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
    string(2) "$a"
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
    string(1) " "
    [2]=>
    int(4)
  }
  [23]=>
  string(1) "}"
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [25]=>
  string(1) "}"
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(6)
  }
}
-- with invlalid PHP open tag & valid tokens --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(27) "<pli
echo "hello world"; ?>"
    [2]=>
    int(1)
  }
}
-- with invalid PHP tags and tokens --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(19) "<PDP display  $a; <"
    [2]=>
    int(1)
  }
}
Done
