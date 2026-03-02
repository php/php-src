--TEST--
Bug #43957 (utf8_decode() bogus conversion on multibyte indicator near end of string)
--FILE--
<?php
  echo utf8_decode('abc'.chr(0xe0));
?>
--EXPECTF--
Deprecated: Function utf8_decode() is deprecated since 8.2, visit the php.net documentation for various alternatives in %s on line %d
abc?
