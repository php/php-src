--TEST--
SPL: ArrayIterator::next() with internal arrays
--FILE--
<?php

$ar = new ArrayObject();

$ar[0] = 1;
$ar[1] = 2;
$ar[2] = 3;
$ar[3] = 4;
$ar[4] = 5;

var_dump($ar);

$it = $ar->getIterator();

$ar->offsetUnset($it->key());
$it->next();

var_dump($it->current());
var_dump($ar);

foreach($it as $k => $v)
{
	$ar->offsetUnset($k+1);
	echo "$k=>$v\n";
}

var_dump($ar);

foreach($it as $k => $v)
{
	$ar->offsetUnset($k);
	echo "$k=>$v\n";
}

var_dump($ar);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
object(ArrayObject)#%d (1) {
  %s"storage"%s"ArrayObject":private]=>
  array(5) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
  }
}
int(3)
object(ArrayObject)#%d (1) {
  %s"storage"%s"ArrayObject":private]=>
  array(4) {
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
  }
}
1=>2
3=>4
object(ArrayObject)#%d (1) {
  %s"storage"%s"ArrayObject":private]=>
  array(2) {
    [1]=>
    int(2)
    [3]=>
    int(4)
  }
}
1=>2
object(ArrayObject)#%d (1) {
  %s"storage"%s"ArrayObject":private]=>
  array(1) {
    [3]=>
    int(4)
  }
}
===DONE===
