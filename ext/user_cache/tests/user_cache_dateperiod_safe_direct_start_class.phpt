--TEST--
UserCache\Cache: a DatePeriod whose start class the fetching process cannot load, or can only load as abstract, is unrestorable
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php
/* The storing child declares concrete start classes; the fetching parent sees one of them only as abstract, the other not at all. */
$cache = UserCache\Cache::getPool('dateperiod-start-class');
$cache->clear();

spl_autoload_register(static function (string $class): void {
    echo "autoload: $class\n";
    if ($class === 'UserCacheAbstractLaterStart') {
        eval('abstract class UserCacheAbstractLaterStart extends DateTimeImmutable {}');
    }
});

$pid = pcntl_fork();
if ($pid < 0) {
    die("fork failed\n");
}
if ($pid === 0) {
    eval('class UserCacheAbstractLaterStart extends DateTimeImmutable {}');
    eval('class UserCacheMissingLaterStart extends DateTimeImmutable {}');
    $stored = $cache->store('abstract-start', new DatePeriod(new UserCacheAbstractLaterStart('2026-01-01'), new DateInterval('P1D'), 1))
        && $cache->store('missing-start', new DatePeriod(new UserCacheMissingLaterStart('2026-01-01'), new DateInterval('P1D'), 1))
        && $cache->store('plain-start', new DatePeriod(new DateTimeImmutable('2026-01-01'), new DateInterval('P1D'), 1));
    exit($stored ? 0 : 1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status));

echo "stored by child:\n";
var_dump($cache->has('abstract-start'), $cache->has('missing-start'), $cache->has('plain-start'));

echo "abstract start class:\n";
var_dump($cache->fetch('abstract-start', 'unrestorable'));
var_dump(class_exists('UserCacheAbstractLaterStart', false));

echo "missing start class:\n";
var_dump($cache->fetch('missing-start', 'unrestorable'));

echo "plain start class:\n";
$plain = $cache->fetch('plain-start', 'unrestorable');
var_dump($plain instanceof DatePeriod);
var_dump($plain->getStartDate()->format('Y-m-d'));

echo "unrestorable entries are dropped:\n";
var_dump($cache->has('abstract-start'), $cache->has('missing-start'), $cache->has('plain-start'));
?>
--EXPECT--
int(0)
stored by child:
bool(true)
bool(true)
bool(true)
abstract start class:
autoload: UserCacheAbstractLaterStart
string(12) "unrestorable"
bool(true)
missing start class:
autoload: UserCacheMissingLaterStart
string(12) "unrestorable"
plain start class:
bool(true)
string(10) "2026-01-01"
unrestorable entries are dropped:
bool(false)
bool(false)
bool(true)
