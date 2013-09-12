--TEST--
Bug #44889 ("<?php" should be one T_OPEN_TAG with short_open_tag=Off)
--INI--
short_open_tag=Off
--FILE--
<?php
var_dump(token_get_all("<?php"));
var_dump(token_get_all("<?xml"));
var_dump(token_get_all("<? php"));
var_dump(token_get_all("<?"));
?>
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(374)
    [1]=>
    string(5) "<?php"
    [2]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(312)
    [1]=>
    string(5) "<?xml"
    [2]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(312)
    [1]=>
    string(6) "<? php"
    [2]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(312)
    [1]=>
    string(2) "<?"
    [2]=>
    int(1)
  }
}
