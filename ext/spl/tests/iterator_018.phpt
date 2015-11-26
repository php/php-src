--TEST--
SPL: InfiniteIterator
--FILE--
<?php

echo "===EmptyIterator===\n";

foreach(new LimitIterator(new InfiniteIterator(new EmptyIterator()), 0, 3) as $key=>$val)
{
	echo "$key=>$val\n";
}

echo "===InfiniteIterator===\n";

$it = new ArrayIterator(array(0 => 'A', 1 => 'B', 2 => 'C', 3 => 'D'));
$it = new InfiniteIterator($it);
$it = new LimitIterator($it, 2, 5);
foreach($it as $val=>$key)
{
	echo "$val=>$key\n";
}

echo "===Infinite/LimitIterator===\n";

$it = new ArrayIterator(array(0 => 'A', 1 => 'B', 2 => 'C', 3 => 'D'));
$it = new LimitIterator($it, 1, 2);
$it = new InfiniteIterator($it);
$it = new LimitIterator($it, 2, 5);
foreach($it as $val=>$key)
{
	echo "$val=>$key\n";
}

?>
===DONE===
<?php exit(0);
--EXPECTF--
===EmptyIterator===
===InfiniteIterator===
2=>C
3=>D
0=>A
1=>B
2=>C
===Infinite/LimitIterator===
1=>B
2=>C
1=>B
2=>C
1=>B
===DONE===
