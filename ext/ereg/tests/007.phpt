--TEST--
Test empty result buffer in reg_replace
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php 
	$a="abcd";
	$b=ereg_replace("abcd","",$a);
	echo "strlen(\$b)=".strlen($b);
?>
--EXPECT--
strlen($b)=0
