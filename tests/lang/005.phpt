--TEST--
Simple If/ElseIf/Else Test
--FILE--
<?php
$a=1; 

if($a==0) {
	echo "bad";
} elseif($a==3) {
	echo "bad";
} else {
	echo "good";
}
?>	
--EXPECT--
good
