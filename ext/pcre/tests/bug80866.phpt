--TEST--
Bug #80866 preg_split ignores limit flag when pattern with \K has 0-width fullstring match
--FILE--
<?php
var_export(preg_split('~.{3}\K~', 'abcdefghijklm', 3));
?>
--EXPECT--
array (
  0 => 'abc',
  1 => 'def',
  2 => 'ghijklm',
)
