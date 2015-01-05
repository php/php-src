--TEST--
escapes in replace string
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","def\\g\\\\hi\\",$a)?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
abcdef\g\\hi\
