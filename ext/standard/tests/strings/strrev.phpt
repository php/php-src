--TEST--
strrev() function
--FILE--
<?php
	$i = 0;
	$str = '';

	while ($i<256) {
		$str .= chr($i++);
	}
	
	var_dump(md5(strrev($str)));
	var_dump(strrev(NULL));
	var_dump(strrev(""));
?>
--EXPECT--
string(32) "ec6df70f2569891eae50321a9179eb82"
string(0) ""
string(0) ""
