--TEST--
Bug #43957 (utf8_decode() bogus conversion on multibyte indicator near end of string)
--FILE--
<?php
  echo utf8_decode('abc'.chr(0xe0));
?>
--EXPECTF--
abc?
