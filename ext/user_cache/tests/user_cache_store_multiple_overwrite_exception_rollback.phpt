--TEST--
UserCache\Cache: a publish exception during storeMultiple() overwrite releases the new key block
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--ENV--
USER_CACHE_DEBUG_FAIL_PUBLISH_CLASS=UCPublishFail
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('store-multiple-overwrite-exception');

class UCPublishFail
{
	public function __construct(public int $n)
	{
	}
}

$cache->clear();
/* The pool's lookup-counter slot is allocated on its first fold. */
$cache->fetch('warm');
UserCache\Cache::getStatus();
$pristine = UserCache\Cache::getStatus()->getUsedMemory();

for ($i = 0; $i < 3; $i++) {
	/* A scalar entry has no payload block, so overwriting it with a graph
	 * value through the bulk path allocates a fresh key block first. */
	var_dump($cache->store('scalar', $i));

	try {
		var_dump($cache->storeMultiple(['scalar' => new UCPublishFail($i)]));
	} catch (Exception $e) {
		echo get_class($e), ': ', $e->getMessage(), "\n";
	}

	var_dump($cache->fetch('scalar'));

	$cache->clear();
	var_dump(UserCache\Cache::getStatus()->getUsedMemory() === $pristine);
}

/* A string entry carries its key inside the combined payload block, so the
 * overwrite takes the combined arm; it must clean up the same way. */
var_dump($cache->store('scalar', 'plain'));

try {
	var_dump($cache->storeMultiple(['scalar' => new UCPublishFail(99)]));
} catch (Exception $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump($cache->fetch('scalar'));

$cache->clear();
var_dump(UserCache\Cache::getStatus()->getUsedMemory() === $pristine);
?>
--EXPECT--
bool(true)
Exception: Debug fault: publish refused for UCPublishFail
int(0)
bool(true)
bool(true)
Exception: Debug fault: publish refused for UCPublishFail
int(1)
bool(true)
bool(true)
Exception: Debug fault: publish refused for UCPublishFail
int(2)
bool(true)
bool(true)
Exception: Debug fault: publish refused for UCPublishFail
string(5) "plain"
bool(true)
