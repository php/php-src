--TEST--
Test token_get_all() function : usage variations - invalid token values
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

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

// with invalid open tag for testing entire source to be unkown token
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
array(29) {
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
    string(6) "struct"
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
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "myStruct"
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
  string(1) "{"
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(2)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "variable"
    [2]=>
    int(%d)
  }
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(%d)
  }
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(%d)
  }
  [11]=>
  string(1) ";"
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(%d)
  }
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "method"
    [2]=>
    int(4)
  }
  [14]=>
  string(1) "("
  [15]=>
  string(1) ")"
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [17]=>
  string(1) "{"
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
    string(7) "display"
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
    string(2) "$a"
    [2]=>
    int(4)
  }
  [22]=>
  string(1) ";"
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [24]=>
  string(1) "}"
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [26]=>
  string(1) "}"
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
-- with invlalid PHP open tag & valid tokens --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(28) "<pli 
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
