--TEST--
SPL: ArrayIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--INI--
allow_call_time_pass_reference=1
--FILE--
<?php

echo "==Normal==\n";

$arr = array(0=>0, 1=>1, 2=>2);
$obj = new ArrayObject($arr);

foreach($obj as $ak=>$av) {
	foreach($obj as $bk=>$bv) {
		if ($ak==0 && $bk==0) {
			$arr[0] = "modify";
		}
		echo "$ak=>$av - $bk=>$bv\n";
	}
}

echo "==UseRef==\n";

$arr = array(0=>0, 1=>1, 2=>2);
$obj = new ArrayObject(&$arr);

foreach($obj as $ak=>$av) {
	foreach($obj as $bk=>$bv) {
		if ($ak==0 && $bk==0) {
			$arr[0] = "modify";
		}
		echo "$ak=>$av - $bk=>$bv\n";
	}
}

echo "==Modify==\n";

$arr = array(0=>0, 1=>1, 2=>2);
$obj = new ArrayObject(&$arr);

foreach($obj as $ak=>$av) {
	foreach($obj as $bk=>$bv) {
		if ($ak==0 && $bk==0) {
			$arr[0] = "modify";
		}
		echo "$ak=>$av - $bk=>$bv\n";
	}
}

echo "==Delete==\n";

$arr = array(0=>0, 1=>1, 2=>2);
$obj = new ArrayObject(&$arr);

foreach($obj as $ak=>$av) {
	foreach($obj as $bk=>$bv) {
		if ($ak==1 && $bk==1) {
			unset($arr[1]);
		}
		echo "$ak=>$av - $bk=>$bv\n";
	}
}

echo "==Change==\n";

$arr = array(0=>0, 1=>1, 2=>2);
$obj = new ArrayObject(&$arr);

foreach($obj as $ak=>$av) {
	foreach($obj as $bk=>$bv) {
		if ($ak==1 && $bk==1) {
			$arr = NULL;
		}
		echo "$ak=>$av - $bk=>$bv\n";
	}
}

echo "Done\n";
?>
--EXPECTF--
==Normal==
0=>0 - 0=>0
0=>0 - 1=>1
0=>0 - 2=>2
1=>1 - 0=>0
1=>1 - 1=>1
1=>1 - 2=>2
2=>2 - 0=>0
2=>2 - 1=>1
2=>2 - 2=>2
==UseRef==
0=>0 - 0=>0
0=>0 - 1=>1
0=>0 - 2=>2
1=>1 - 0=>modify
1=>1 - 1=>1
1=>1 - 2=>2
2=>2 - 0=>modify
2=>2 - 1=>1
2=>2 - 2=>2
==Modify==
0=>0 - 0=>0
0=>0 - 1=>1
0=>0 - 2=>2
1=>1 - 0=>modify
1=>1 - 1=>1
1=>1 - 2=>2
2=>2 - 0=>modify
2=>2 - 1=>1
2=>2 - 2=>2
==Delete==
0=>0 - 0=>0
0=>0 - 1=>1
0=>0 - 2=>2
1=>1 - 0=>0
1=>1 - 1=>1

Notice: main(): ArrayIterator::next(): Array was modified outside object and internal position is no longer valid in %sarray_004.php on line %d
1=>1 - 0=>0
1=>1 - 2=>2

Notice: main(): ArrayIterator::next(): Array was modified outside object and internal position is no longer valid in %sarray_004.php on line %d
0=>0 - 0=>0
0=>0 - 2=>2
2=>2 - 0=>0
2=>2 - 2=>2
==Change==
0=>0 - 0=>0
0=>0 - 1=>1
0=>0 - 2=>2
1=>1 - 0=>0
1=>1 - 1=>1

Notice: main(): ArrayIterator::current(): Array was modified outside object and is no longer an array in %sarray_004.php on line %d

Notice: main(): ArrayIterator::valid(): Array was modified outside object and is no longer an array in %sarray_004.php on line %d

Notice: main(): ArrayIterator::current(): Array was modified outside object and is no longer an array in %sarray_004.php on line %d

Notice: main(): ArrayIterator::valid(): Array was modified outside object and is no longer an array in %sarray_004.php on line %d
Done
