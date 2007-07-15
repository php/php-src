--TEST--
escapes in replace string
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","def\\g\\\\hi\\",$a)?>
--EXPECT--
abcdef\g\\hi\
