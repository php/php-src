--TEST--
private(namespace) virtual property with hooks - same namespace
--FILE--
<?php

namespace App\Cache;

class CacheManager {
    private array $storage = [];
    
    private(namespace) string $lastKey {
        get => end($this->storage) ?: 'none';
        set {
            $this->storage[] = $value;
        }
    }
}

class CacheMonitor {
    public function track(CacheManager $cache): void {
        // Same namespace - should work
        var_dump($cache->lastKey);
        $cache->lastKey = "key1";
        var_dump($cache->lastKey);
        $cache->lastKey = "key2";
        var_dump($cache->lastKey);
    }
}

$monitor = new CacheMonitor();
$cache = new CacheManager();
$monitor->track($cache);

?>
--EXPECT--
string(4) "none"
string(4) "key1"
string(4) "key2"
