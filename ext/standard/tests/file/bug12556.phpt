--TEST--
Bug #12556 (fgetcsv() ignores lengths when quotes not closed)
--FILE--
<?php
$fp = fopen(dirname(__FILE__)."/test.csv", "r");
while($line = fgetcsv($fp, 24)) {
	$line = str_replace("\x0d\x0a", "\x0a", $line);
	var_dump($line);
}
fclose($fp);
?>
--EXPECT--
array(4) {
  [0]=>
  unicode(1) "6"
  [1]=>
  unicode(1) "7"
  [2]=>
  unicode(1) "8"
  [3]=>
  unicode(5) "line1"
}
array(4) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(1) "2"
  [2]=>
  unicode(1) "3"
  [3]=>
  unicode(186) "line2
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
2,4,5,line3
"
}

