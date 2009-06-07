--TEST--
Foreach loop tests - modifying the array during the loop.
--FILE--
<?php

echo "\nDirectly changing array values.\n";
$a = array("original.1","original.2","original.3");
foreach ($a as $k=>$v) {
	$a[$k]="changed.$k";
	var_dump($v);
}
var_dump($a);

echo "\nModifying the foreach \$value.\n";
$a = array("original.1","original.2","original.3");
foreach ($a as $k=>$v) {
	$v="changed.$k";
}
var_dump($a);


echo "\nModifying the foreach &\$value.\n";
$a = array("original.1","original.2","original.3");
foreach ($a as $k=>&$v) {
	$v="changed.$k";
}
var_dump($a);

echo "\nPushing elements onto an unreferenced array.\n";
$a = array("original.1","original.2","original.3");
$counter=0;
foreach ($a as $v) {
	array_push($a, "new.$counter");

	//avoid infinite loop if test is failing
    if ($counter++>10) {
    	echo "Loop detected\n";
    	break;    	
    }
}
var_dump($a);

echo "\nPushing elements onto an unreferenced array, using &\$value.\n";
$a = array("original.1","original.2","original.3");
$counter=0;
foreach ($a as &$v) {
	array_push($a, "new.$counter");

	//avoid infinite loop if test is failing
    if ($counter++>10) {
    	echo "Loop detected\n";
    	break;    	
    }	
}
var_dump($a);

echo "\nPopping elements off an unreferenced array.\n";
$a = array("original.1","original.2","original.3");
foreach ($a as $v) {
	array_pop($a);
	var_dump($v);	
}
var_dump($a);

echo "\nPopping elements off an unreferenced array, using &\$value.\n";
$a = array("original.1","original.2","original.3");
foreach ($a as &$v) {
	array_pop($a);
	var_dump($v);
}
var_dump($a);

?>
--EXPECT--

Directly changing array values.
unicode(10) "original.1"
unicode(10) "original.2"
unicode(10) "original.3"
array(3) {
  [0]=>
  unicode(9) "changed.0"
  [1]=>
  unicode(9) "changed.1"
  [2]=>
  unicode(9) "changed.2"
}

Modifying the foreach $value.
array(3) {
  [0]=>
  unicode(10) "original.1"
  [1]=>
  unicode(10) "original.2"
  [2]=>
  unicode(10) "original.3"
}

Modifying the foreach &$value.
array(3) {
  [0]=>
  unicode(9) "changed.0"
  [1]=>
  unicode(9) "changed.1"
  [2]=>
  &unicode(9) "changed.2"
}

Pushing elements onto an unreferenced array.
array(6) {
  [0]=>
  unicode(10) "original.1"
  [1]=>
  unicode(10) "original.2"
  [2]=>
  unicode(10) "original.3"
  [3]=>
  unicode(5) "new.0"
  [4]=>
  unicode(5) "new.1"
  [5]=>
  unicode(5) "new.2"
}

Pushing elements onto an unreferenced array, using &$value.
Loop detected
array(15) {
  [0]=>
  unicode(10) "original.1"
  [1]=>
  unicode(10) "original.2"
  [2]=>
  unicode(10) "original.3"
  [3]=>
  unicode(5) "new.0"
  [4]=>
  unicode(5) "new.1"
  [5]=>
  unicode(5) "new.2"
  [6]=>
  unicode(5) "new.3"
  [7]=>
  unicode(5) "new.4"
  [8]=>
  unicode(5) "new.5"
  [9]=>
  unicode(5) "new.6"
  [10]=>
  unicode(5) "new.7"
  [11]=>
  &unicode(5) "new.8"
  [12]=>
  unicode(5) "new.9"
  [13]=>
  unicode(6) "new.10"
  [14]=>
  unicode(6) "new.11"
}

Popping elements off an unreferenced array.
unicode(10) "original.1"
unicode(10) "original.2"
unicode(10) "original.3"
array(0) {
}

Popping elements off an unreferenced array, using &$value.
unicode(10) "original.1"
unicode(10) "original.2"
array(1) {
  [0]=>
  unicode(10) "original.1"
}