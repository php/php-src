--TEST--
Bug #78929 (plus signs in cookie values are converted to spaces)
--INI--
max_input_vars=1000
filter.default=unsafe_raw
--COOKIE--
RFC6265=#$%&'()*+-./0123456789<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~!
--FILE--
<?php
var_dump($_COOKIE);
?>
--EXPECT--
array(1) {
  ["RFC6265"]=>
  string(89) "#$%&'()*+-./0123456789<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~!"
}
