--TEST--
nonexisting back reference
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php $a="abc123";
  echo ereg_replace("123",'def\1ghi',$a)?>
--EXPECT--
abcdef\1ghi
