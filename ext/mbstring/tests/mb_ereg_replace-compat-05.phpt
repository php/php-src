--TEST--
mb_ereg_replace() compatibility test 5 (counterpart: ext/standard/tests/reg/007.phpt)
--POST--
--GET--
--FILE--
<?php 
	$a="abcd";
	$b=mb_ereg_replace("abcd","",$a);
	echo "strlen(\$b)=".strlen($b);
?>
--EXPECT--
strlen($b)=0
