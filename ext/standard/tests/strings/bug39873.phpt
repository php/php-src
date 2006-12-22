--TEST--
Bug #39873 (number_format() breaks with locale & decimal points)
--SKIPIF--
<?php
if (!setlocale(LC_ALL, "ita","it","Italian","it_IT","it_IT.ISO8859-1","it_IT.ISO_8859-1")) {
        die("skip locale needed for this test is not supported on this platform");
}
?>
--FILE--
<?php
	setlocale(LC_ALL, "ita","it","Italian","it_IT","it_IT.ISO8859-1","it_IT.ISO_8859-1");
	$num = 0+"1234.56";  
	echo number_format($num,2);
	echo "\n";
?>
--EXPECT--
1,234.56
