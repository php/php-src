--TEST--
UserCache\Cache: stale enum case names decode safely instead of crashing
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
zend.exception_ignore_args=0
--FILE--
<?php
$cache = UserCache\Cache::getPool('stale-enum-case');
$cache->clear();

/*
 * A bare enum value routes through user_cache_shared_graph_decode_enum(),
 * while an enum passed as a trace argument of an internal exception (which
 * carries __wakeup but no __serialize) routes through the serdes encoder and
 * user_cache_serdes_decode_enum(). Storing both covers both decoders.
 */
function user_cache_stale_enum_capture($case): RuntimeException
{
    return new RuntimeException('captured');
}

$names = ['missing', 'const', 'kept', 'backed'];

$pid = pcntl_fork();
if ($pid === 0) {
    /* Enum definitions in effect when the entries are stored. */
    enum UserCacheStaleEnumMissing { case Active; case Gone; }
    enum UserCacheStaleEnumConst { case Active; case Renamed; }
    enum UserCacheStaleEnumKept { case Active; }
    enum UserCacheStaleEnumBacked: string { case Active = 'active'; }

    $cases = [
        'missing' => UserCacheStaleEnumMissing::Gone,
        'const'   => UserCacheStaleEnumConst::Renamed,
        'kept'    => UserCacheStaleEnumKept::Active,
        'backed'  => UserCacheStaleEnumBacked::Active,
    ];
    foreach ($cases as $name => $case) {
        $cache->store("$name-graph", $case);
        $cache->store("$name-serdes", user_cache_stale_enum_capture($case));
    }
    exit(0);
}

pcntl_waitpid($pid, $status);

/* Enum definitions in effect when the entries are fetched (stale). */
enum UserCacheStaleEnumMissing { case Active; }
enum UserCacheStaleEnumConst { case Active; const Renamed = 'renamed'; }
enum UserCacheStaleEnumKept { case Active; }
enum UserCacheStaleEnumBacked: string { case Active = 'active'; }

foreach ($names as $name) {
    echo "$name-graph: ";
    var_dump($cache->fetch("$name-graph", 'default'));
    echo "$name-graph has: ";
    var_dump($cache->has("$name-graph"));

    echo "$name-serdes: ";
    $value = $cache->fetch("$name-serdes", 'default');
    if ($value instanceof RuntimeException) {
        var_dump($value->getTrace()[0]['args'][0]);
    } else {
        var_dump($value);
    }
    echo "$name-serdes has: ";
    var_dump($cache->has("$name-serdes"));
}
?>
--EXPECT--
missing-graph: string(7) "default"
missing-graph has: bool(false)
missing-serdes: string(7) "default"
missing-serdes has: bool(false)
const-graph: string(7) "default"
const-graph has: bool(false)
const-serdes: string(7) "default"
const-serdes has: bool(false)
kept-graph: enum(UserCacheStaleEnumKept::Active)
kept-graph has: bool(true)
kept-serdes: enum(UserCacheStaleEnumKept::Active)
kept-serdes has: bool(true)
backed-graph: enum(UserCacheStaleEnumBacked::Active)
backed-graph has: bool(true)
backed-serdes: enum(UserCacheStaleEnumBacked::Active)
backed-serdes has: bool(true)
