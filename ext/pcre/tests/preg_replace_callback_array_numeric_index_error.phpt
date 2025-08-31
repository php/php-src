--TEST--
preg_replace_callback_array() invalid pattern
--FILE--
<?php
preg_replace_callback_array(
    [42 => function () {}],
    'a',
);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: preg_replace_callback_array(): Argument #1 ($pattern) must contain only string patterns as keys in %s:%d
Stack trace:
#0 %s(%d): preg_replace_callback_array(Array, 'a')
#1 {main}
  thrown in %s on line %d
