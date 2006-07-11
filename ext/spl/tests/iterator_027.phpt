--TEST--
SPL: CachingIterator::FULL_CACHE
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$ar = array(1, 2, array(31, 32, array(331)), 4);

$it = new RecursiveArrayIterator($ar);
$it = new RecursiveIteratorIterator($it);
$it = new CachingIterator($it, CachingIterator::FULL_CACHE);

foreach($it as $k=>$v)
{
	echo "$k=>$v\n";
}

echo "===CHECK===\n";

for ($i = 0; $i < 4; $i++)
{
	if (isset($it[$i]))
	{
		var_dump($i, $it[$i]);
	}
}

$it[2] = 'foo';
$it[3] = 'bar';
$it['baz'] = '25';

var_dump($it[2]);
var_dump($it[3]);
var_dump($it['baz']);

unset($it[0]);
unset($it[2]);
unset($it['baz']);

var_dump(isset($it[0])); // unset
var_dump(isset($it[1])); // still present
var_dump(isset($it[2])); // unset
var_dump(isset($it[3])); // still present
var_dump(isset($it['baz']));

echo "===REWIND===\n";

$it->rewind(); // cleans and reads first element
var_dump(isset($it[0])); // pre-fetched
var_dump(isset($it[1])); // deleted
var_dump(isset($it[2])); // unset
var_dump(isset($it[3])); // deleted

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
0=>1
1=>2
0=>31
1=>32
0=>331
3=>4
===CHECK===
int(0)
int(331)
int(1)
int(32)
int(3)
int(4)
string(3) "foo"
string(3) "bar"
string(2) "25"
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
===REWIND===
bool(true)
bool(false)
bool(false)
bool(false)
===DONE===
