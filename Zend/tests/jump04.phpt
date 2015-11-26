--TEST--
jump 04: goto from loop (backward)
--FILE--
<?php
$s = "X";
echo "1: ok\n";	
L1: if ($s != "X") {
	echo "4: ok\n";	
} else {	
	echo "2: ok\n";	
	while ($s != "XXX") {
		echo "3: ok\n";	
		$s .= "X";
		goto L1;
		echo "bug\n";	
	}
	echo "bug\n";	
}
?>
--EXPECT--
1: ok
2: ok
3: ok
4: ok
