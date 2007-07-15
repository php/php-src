--TEST--
RegReplace test 1
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","def",$a)?>
--EXPECT--
abcdef
