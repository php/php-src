--TEST--
UserCache\Cache: __wakeup()-only objects keep outer shared graph identity
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('wakeup-only-identity');
$cache->clear();

class WakeupOnlyIdentityHolder
{
    public static int $wakeupCalls = 0;
    public object $child;

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
    }
}

class WakeupOnlyIdentityChild
{
    public int $value = 1;
}

$child = new WakeupOnlyIdentityChild();
$holder = new WakeupOnlyIdentityHolder();
$holder->child = $child;

$native = unserialize(serialize(['holder' => $holder, 'child' => $child]));
var_dump($native['holder']->child === $native['child']);

WakeupOnlyIdentityHolder::$wakeupCalls = 0;

var_dump($cache->store('graph', ['holder' => $holder, 'child' => $child]));

$first = $cache->fetch('graph');
var_dump($first['holder']->child === $first['child']);
$first['holder']->child->value = 42;
var_dump($first['child']->value);
var_dump(WakeupOnlyIdentityHolder::$wakeupCalls);

$second = $cache->fetch('graph');
var_dump($second['holder']->child === $second['child']);
var_dump($second['child']->value);
var_dump(WakeupOnlyIdentityHolder::$wakeupCalls);
var_dump($first['holder'] !== $second['holder']);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
int(42)
int(1)
bool(true)
int(1)
int(2)
bool(true)
