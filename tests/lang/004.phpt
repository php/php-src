--TEST--
Simple If/Else Test
--POST--
--GET--
--FILE--
<?php
$a=1; 
if($a==0) {
	echo "bad";
} else {
	echo "good";
}
?>	
--EXPECT--
good
