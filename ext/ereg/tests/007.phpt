--TEST--
Test empty result buffer in reg_replace
--FILE--
<?php 
	$a="abcd";
	$b=ereg_replace("abcd","",$a);
	echo "strlen(\$b)=".strlen($b);
?>
--EXPECT--
strlen($b)=0
