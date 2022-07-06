--TEST--
testing the behavior of stream_filter_register
--CREDITS--
Robrecht Plaisier <php@mcq8.be>
User Group: PHP-WVL & PHPGent #PHPTestFest
--FILE--
<?php
class foo extends php_user_filter {
  function filter($in, $out, &$consumed, $closing) {
  }
}

class bar extends php_user_filter {
  function filter($in, $out, &$consumed, $closing) {
  }
}

var_dump(stream_filter_register("myfilter","foo"));
var_dump(stream_filter_register("myfilter","bar"));
var_dump(stream_filter_register("foo","foo"));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
