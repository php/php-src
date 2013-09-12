--TEST--
Bug #44889 ("<?php" should be one T_OPEN_TAG with short_open_tag=On)
--INI--
short_open_tag=On
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
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(374)
    [1]=>
    string(2) "<?"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(308)
    [1]=>
    string(3) "xml"
    [2]=>
    int(1)
  }
}
array(3) {
  [0]=>
  array(3) {
    [0]=>
    int(374)
    [1]=>
    string(2) "<?"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(377)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(308)
    [1]=>
    string(3) "php"
    [2]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(374)
    [1]=>
    string(2) "<?"
    [2]=>
    int(1)
  }
}
