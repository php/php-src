--TEST--
Test empty result buffer in reg_replace
--POST--
--GET--
--FILE--
<?php 
	$a="abcd";
	$b=ereg_replace("abcd","",$a);
	echo "strlen(\$b)=".strlen($b);
?>
--EXPECT--
strlen($b)=0
