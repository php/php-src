--TEST--
Simple If/Else Test
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
