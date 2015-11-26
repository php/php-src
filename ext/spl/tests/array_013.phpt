--TEST--
SPL: ArrayIterator::append
--FILE--
<?php

if (!class_exists('NoRewindIterator', false))
{
	require_once(dirname(__FILE__) . '/../examples/norewinditerator.inc');
}                                               

echo "===Array===\n";

$a = array(0 => 'zero', 1 => 'one', 2 => 'two');
$it = new ArrayIterator($a);

foreach($it as $key => $val)
{
	echo "$key=>$val\n";
}

echo "===Append===\n";

$it->append('three');
$it->append('four');

foreach(new NoRewindIterator($it) as $key => $val)
{
	echo "$key=>$val\n";
}

echo "===Object===\n";

class test
{
	public $zero = 0;
	protected $pro;
	public $one = 1;
	private $pri;
	public $two = 2;
}

$o = new test;
$it = new ArrayIterator($o);

foreach($it as $key => $val)
{
	echo "$key=>$val\n";
}

echo "===Append===\n";

$it->append('three');
$it->append('four');

foreach(new NoRewindIterator($it) as $key => $val)
{
	echo "$key=>$val\n";
}

var_dump($o->{0}); /* doesn't wotk anyway */

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===Array===
0=>zero
1=>one
2=>two
===Append===
3=>three
4=>four
===Object===
zero=>0
one=>1
two=>2
===Append===

Catchable fatal error: ArrayIterator::append(): Cannot append properties to objects, use ArrayIterator::offsetSet() instead in %sarray_013.php on line %d
