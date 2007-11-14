--TEST--
SPL: ArrayIterator and foreach reference
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--INI--
allow_call_time_pass_reference=1
--FILE--
<?php

echo "==Normal==\n";

$arr = array(0=>0, 1=>1, 2=>2);
$obj = new ArrayObject($arr);

foreach($obj as $ak=>&$av) {
	foreach($obj as $bk=>&$bv) {
		if ($ak==0 && $bk==0) {
			$bv = "modify";
		}
		echo "$ak=>$av - $bk=>$bv\n";
	}
}

echo "==UseRef==\n";

$arr = array(0=>0, 1=>1, 2=>2);
$obj = new ArrayObject(&$arr);

foreach($obj as $ak=>&$av) {
	foreach($obj as $bk=>&$bv) {
		if ($ak==0 && $bk==0) {
			$bv = "modify";
		}
		echo "$ak=>$av - $bk=>$bv\n";
	}
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
==Normal==
0=>modify - 0=>modify
0=>modify - 1=>1
0=>modify - 2=>2
1=>1 - 0=>modify
1=>1 - 1=>1
1=>1 - 2=>2
2=>2 - 0=>modify
2=>2 - 1=>1
2=>2 - 2=>2
==UseRef==
0=>modify - 0=>modify
0=>modify - 1=>1
0=>modify - 2=>2
1=>1 - 0=>modify
1=>1 - 1=>1
1=>1 - 2=>2
2=>2 - 0=>modify
2=>2 - 1=>1
2=>2 - 2=>2
===DONE===
