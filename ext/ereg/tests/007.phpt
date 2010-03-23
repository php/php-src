--TEST--
Test empty result buffer in reg_replace
--FILE--
<?php 
	$a="abcd";
	$b=ereg_replace("abcd","",$a);
	echo "strlen(\$b)=".strlen($b);
?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
strlen($b)=0
