--TEST--
UserCache\Cache: clear() only affects its own pool
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$a = UserCache\Cache::getPool('clear-scope-a');
$b = UserCache\Cache::getPool('clear-scope-b');

var_dump($a->store('shared-key', 'from-a'));
var_dump($a->store('a-only', 1));
var_dump($b->store('shared-key', 'from-b'));
var_dump($b->store('b-only', 2));

var_dump($a->clear());

var_dump($a->has('shared-key'));
var_dump($a->has('a-only'));
var_dump($a->getPoolStatus()->getEntryCount());

var_dump($b->fetch('shared-key'));
var_dump($b->fetch('b-only'));
var_dump($b->getPoolStatus()->getEntryCount());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
int(0)
string(6) "from-b"
int(2)
int(2)
