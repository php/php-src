--TEST--
Bug #26463 (token_get_all() does not correctly handle semicolons after T_END_HEREDOC)
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
array(17) {
  [0]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php
"
  }
  [1]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$x"
  }
  [2]=>
  string(1) "="
  [3]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<<<DD
"
  }
  [4]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "jhdsjkfhjdsh
"
  }
  [5]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "DD"
  }
  [6]=>
  string(1) "."
  [7]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) """"
  }
  [8]=>
  string(1) ";"
  [9]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
  }
  [10]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [11]=>
  string(1) "="
  [12]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "<<<DDDD
"
  }
  [13]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "jhdsjkfhjdsh
"
  }
  [14]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "DDDD"
  }
  [15]=>
  string(1) ";"
  [16]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
  }
}
