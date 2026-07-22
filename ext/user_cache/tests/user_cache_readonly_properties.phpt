--TEST--
UserCache\Cache: readonly and inherited readonly properties survive the round trip
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('readonly');

class UserCacheReadonlyBase
{
    public function __construct(
        public readonly string $baseName,
        protected readonly int $baseCount,
    ) {}

    public function describe(): string
    {
        return $this->baseName . ':' . $this->baseCount;
    }
}

class UserCacheReadonlyChild extends UserCacheReadonlyBase
{
    public function __construct(
        string $baseName,
        int $baseCount,
        public readonly array $items,
    ) {
        parent::__construct($baseName, $baseCount);
    }
}

$child = new UserCacheReadonlyChild('alpha', 7, ['a', 'b']);

var_dump($cache->store('child', $child));

$fetched = $cache->fetch('child');
var_dump($fetched instanceof UserCacheReadonlyChild);
var_dump($fetched->describe());
var_dump($fetched->baseName);
var_dump($fetched->items);

try {
    $fetched->baseName = 'changed';
} catch (Error $e) {
    echo get_class($e), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
string(7) "alpha:7"
string(5) "alpha"
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
Error
