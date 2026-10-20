--TEST--
MessageFormatter::parse() with PHP_INT_MIN on 64-bit platform
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--FILE--
<?php

$fmt = new MessageFormatter('en_US', '{0,number,integer}');
$parsed = $fmt->parse('-9,223,372,036,854,775,808');
var_dump($parsed);

$parsed = MessageFormatter::parseMessage('en_US', '{0,number,integer}', '-9,223,372,036,854,775,808');
var_dump($parsed);

?>
--EXPECT--
array(1) {
  [0]=>
  int(-9223372036854775808)
}
array(1) {
  [0]=>
  int(-9223372036854775808)
}
