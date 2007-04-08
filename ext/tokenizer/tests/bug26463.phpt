--TEST--
Bug #26463 (token_get_all() does not correctly handle semicolons after T_END_HEREDOC)
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
$str = '<?php
$x=<<<DD
jhdsjkfhjdsh
DD
."";
$a=<<<DDDD
jhdsjkfhjdsh
DDDD;
?>';
var_dump(token_get_all($str));
?>
--EXPECTF--
array(19) {
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
    string(2) "$x"
    [2]=>
    int(2)
  }
  [2]=>
  string(1) "="
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<<<DD
"
    [2]=>
    int(2)
  }
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "jhdsjkfhjdsh
"
    [2]=>
    int(3)
  }
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "DD"
    [2]=>
    int(4)
  }
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [7]=>
  string(1) "."
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) """"
    [2]=>
    int(5)
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
    int(5)
  }
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(6)
  }
  [12]=>
  string(1) "="
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "<<<DDDD
"
    [2]=>
    int(6)
  }
  [14]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "jhdsjkfhjdsh
"
    [2]=>
    int(7)
  }
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "DDDD"
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
    string(2) "?>"
    [2]=>
    int(9)
  }
}
