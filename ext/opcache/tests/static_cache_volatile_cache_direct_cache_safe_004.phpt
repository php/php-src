--TEST--
OPcache direct cache handlers cover SPL edge branches
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

$plainFixed = new SplFixedArray(2);
$plainFixed[0] = 'first';
$plainFixed[1] = 'second';

var_dump(OPcache\VolatileCache::set('safe_direct_plain_fixed', $plainFixed));

$plainFixedCopy = OPcache\VolatileCache::get('safe_direct_plain_fixed');
var_dump($plainFixedCopy instanceof SplFixedArray);
var_dump($plainFixedCopy->getSize());
var_dump($plainFixedCopy[0]);
var_dump($plainFixedCopy[1]);

$emptyFixed = new SplFixedArray(0);

var_dump(OPcache\VolatileCache::set('safe_direct_empty_fixed', $emptyFixed));

$emptyFixedCopy = OPcache\VolatileCache::get('safe_direct_empty_fixed');
var_dump($emptyFixedCopy instanceof SplFixedArray);
var_dump($emptyFixedCopy->getSize());

$flagged = new ArrayObject(['name' => 'val'], ArrayObject::ARRAY_AS_PROPS);

var_dump(OPcache\VolatileCache::set('safe_direct_array_props', $flagged));

$flaggedCopy = OPcache\VolatileCache::get('safe_direct_array_props');
var_dump($flaggedCopy instanceof ArrayObject);
var_dump($flaggedCopy->getFlags());
var_dump($flaggedCopy->name);
var_dump($flaggedCopy['name']);
var_dump($flaggedCopy->getIteratorClass());

error_reporting(E_ALL & ~E_DEPRECATED);

class SelfBackedCollection extends ArrayObject
{
	public int $slot = 0;
}

$selfBacked = new SelfBackedCollection(['alpha' => 1]);
$selfBacked->exchangeArray($selfBacked);
$selfBacked['slot'] = 7;

var_dump(OPcache\VolatileCache::set('safe_direct_self_array_object', $selfBacked));

$selfBackedCopy = OPcache\VolatileCache::get('safe_direct_self_array_object');
var_dump($selfBackedCopy instanceof SelfBackedCollection);
var_dump($selfBackedCopy->slot);
var_dump($selfBackedCopy->count());
var_dump(isset($selfBackedCopy['slot']));
var_dump($selfBackedCopy->getIteratorClass());

$backedIterator = (new ArrayObject(['x' => 1]))->getIterator();

var_dump(OPcache\VolatileCache::set('safe_direct_object_backed_iterator', $backedIterator));

$backedIteratorCopy = OPcache\VolatileCache::get('safe_direct_object_backed_iterator');
$backedIteratorCopy->rewind();
var_dump($backedIteratorCopy instanceof ArrayIterator);
var_dump($backedIteratorCopy->count());
var_dump($backedIteratorCopy->key());
var_dump($backedIteratorCopy->current());

?>
--EXPECT--
bool(true)
bool(true)
int(2)
string(5) "first"
string(6) "second"
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
int(2)
string(3) "val"
string(3) "val"
string(13) "ArrayIterator"
bool(true)
bool(true)
int(7)
int(1)
bool(true)
string(13) "ArrayIterator"
bool(true)
bool(true)
int(1)
string(1) "x"
int(1)
