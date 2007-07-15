--TEST--
\0 back reference
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","def\\0ghi",$a)?>
--EXPECT--
abcdef123ghi
