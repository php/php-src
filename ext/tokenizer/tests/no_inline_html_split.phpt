--TEST--
Inline HTML should not be split at partial PHP tags
--EXTENSIONS--
tokenizer
--INI--
short_open_tag=0
--FILE--
<?php

var_dump(token_get_all(<<<'PHP'
Foo<?phpBar
PHP));

?>
--EXPECTF--
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(11) "Foo<?phpBar"
    [2]=>
    int(1)
  }
}
