--TEST--
UserCache\Cache: pool status reports per-pool memory usage
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php

$a = UserCache\Cache::getPool('mem-a');
$b = UserCache\Cache::getPool('mem-b');

$empty = $a->getPoolStatus();
var_dump($empty->getEntryCount());
var_dump($empty->getEntryKeys());
var_dump($empty->getUsedMemory());

var_dump($a->store('int', 123));
$afterInt = $a->getPoolStatus();
var_dump($afterInt->getEntryCount());
var_dump($afterInt->getEntryKeys());
var_dump($afterInt->getUsedMemory() > 0);

$intMemory = $afterInt->getUsedMemory();
var_dump($a->store('str', str_repeat('x', 100)));
$afterString = $a->getPoolStatus();
$keys = $afterString->getEntryKeys();
sort($keys);
var_dump($afterString->getEntryCount());
var_dump($keys);
var_dump($afterString->getUsedMemory() > $intMemory);
$stringMemory = $afterString->getUsedMemory();

var_dump($b->store('int', 123));
var_dump($b->store('str', str_repeat('x', 100)));
$bStatus = $b->getPoolStatus();
var_dump($bStatus->getUsedMemory() === $afterString->getUsedMemory());

var_dump($a->store('large', str_repeat('y', 4096)));
$afterLarge = $a->getPoolStatus();
var_dump($afterLarge->getEntryCount());
var_dump($afterLarge->getUsedMemory() > $stringMemory);
/* Status objects are snapshots, not live views. */
var_dump($afterString->getUsedMemory() === $afterLarge->getUsedMemory());
var_dump($b->getPoolStatus()->getUsedMemory() === $bStatus->getUsedMemory());

var_dump($a->delete('large'));
$afterDelete = $a->getPoolStatus();
var_dump($afterDelete->getEntryCount());
var_dump($afterDelete->getUsedMemory() === $stringMemory);

var_dump($a->clear());
$afterClear = $a->getPoolStatus();
var_dump($afterClear->getEntryCount());
var_dump($afterClear->getEntryKeys());
var_dump($afterClear->getUsedMemory());
?>
--EXPECT--
int(0)
array(0) {
}
int(0)
bool(true)
int(1)
array(1) {
  [0]=>
  string(3) "int"
}
bool(true)
bool(true)
int(2)
array(2) {
  [0]=>
  string(3) "int"
  [1]=>
  string(3) "str"
}
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(3)
bool(true)
bool(false)
bool(true)
bool(true)
int(2)
bool(true)
bool(true)
int(0)
array(0) {
}
int(0)
