--TEST--
ereg_replace single-quote test
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php $a="\\'test";
  echo ereg_replace("\\\\'","'",$a)
?>
--EXPECT--
'test
