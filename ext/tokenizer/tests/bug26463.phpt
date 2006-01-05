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
  array(2) {
    [0]=>
    int(354)
    [1]=>
    string(6) "<?php
"
  }
  [1]=>
  array(2) {
    [0]=>
    int(307)
    [1]=>
    string(2) "$x"
  }
  [2]=>
  string(1) "="
  [3]=>
  array(2) {
    [0]=>
    int(358)
    [1]=>
    string(6) "<<<DD
"
  }
  [4]=>
  array(2) {
    [0]=>
    int(305)
    [1]=>
    string(13) "jhdsjkfhjdsh
"
  }
  [5]=>
  array(2) {
    [0]=>
    int(359)
    [1]=>
    string(2) "DD"
  }
  [6]=>
  array(2) {
    [0]=>
    int(357)
    [1]=>
    string(1) "
"
  }
  [7]=>
  string(1) "."
  [8]=>
  array(2) {
    [0]=>
    int(313)
    [1]=>
    string(2) """"
  }
  [9]=>
  string(1) ";"
  [10]=>
  array(2) {
    [0]=>
    int(357)
    [1]=>
    string(1) "
"
  }
  [11]=>
  array(2) {
    [0]=>
    int(307)
    [1]=>
    string(2) "$a"
  }
  [12]=>
  string(1) "="
  [13]=>
  array(2) {
    [0]=>
    int(358)
    [1]=>
    string(8) "<<<DDDD
"
  }
  [14]=>
  array(2) {
    [0]=>
    int(305)
    [1]=>
    string(13) "jhdsjkfhjdsh
"
  }
  [15]=>
  array(2) {
    [0]=>
    int(359)
    [1]=>
    string(4) "DDDD"
  }
  [16]=>
  string(1) ";"
  [17]=>
  array(2) {
    [0]=>
    int(357)
    [1]=>
    string(1) "
"
  }
  [18]=>
  array(2) {
    [0]=>
    int(356)
    [1]=>
    string(2) "?>"
  }
}
