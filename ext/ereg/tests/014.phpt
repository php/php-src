--TEST--
backreferences not replaced recursively
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php $a="a\\2bxc";
  echo ereg_replace("a(.*)b(.*)c","\\1",$a)?>
--EXPECT--
\2
