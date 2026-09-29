--TEST--
UserCache\Cache: request-local slots built from another process's stores promote lazily and survive re-entrant eviction, and a forked child pins inherited prototypes independently
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php
function prototype_fork_pins(): void
{
    $cache = UserCache\Cache::getPool('prototype-fork-pins');
    $cache->clear();
    foreach (['one', 'two', 'cold'] as $key) {
        $cache->store($key, (object) ['values' => range(1, 64)]);
        if ($key !== 'cold') {
            $cache->fetch($key);
            $cache->fetch($key);
        }
    }
    echo 'parent pins: ', UserCache\Cache::getStatus()->getGraphPinnedReferences(), "\n";

    $pid = pcntl_fork();
    if ($pid < 0) {
        die("fork failed\n");
    }
    if ($pid === 0) {
        /* The cold decode first refreshes the graph-reference owner PID. Each
         * inherited prototype must still acquire its own independent pin. */
        foreach (['cold', 'one', 'two'] as $key) {
            $fetched = $cache->fetch($key);
            echo $key, ' pins: ', UserCache\Cache::getStatus()->getGraphPinnedReferences(), "\n";
            var_dump($fetched->values === range(1, 64));
        }
        exit(0);
    }
    pcntl_waitpid($pid, $status);
    var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
    echo 'remaining parent pins: ', UserCache\Cache::getStatus()->getGraphPinnedReferences(), "\n";
}

echo "prototype fork pins:\n";
prototype_fork_pins();

echo "\nrequest-local slots:\n";
/* A graph stored by a child is promoted only after the first fetch and stays isolated per fetch. */
class UserCacheDeferredPromotionDate extends DateTime
{
    private int $cachedSelfId;

    public function __construct(string $time, DateTimeZone $timezone)
    {
        parent::__construct($time, $timezone);
        $this->cachedSelfId = spl_object_id($this);
    }

    public function cachedSelfId(): int
    {
        return $this->cachedSelfId;
    }
}

$cache = UserCache\Cache::getPool('deferred-promotion');
$cache->clear();

$pid = pcntl_fork();
if ($pid === 0) {
    $date = new UserCacheDeferredPromotionDate('2026-07-03 12:34:56.123456', new DateTimeZone('Asia/Tokyo'));
    $payload = [
        'first' => $date,
        'second' => $date,
        'nested' => ['label' => 'original'],
        'storedId' => $date->cachedSelfId(),
    ];

    var_dump($cache->store('payload', $payload));
    exit(0);
}

if ($pid < 0) {
    echo "pcntl_fork() failed\n";
    exit(1);
}

pcntl_waitpid($pid, $status);

$first = $cache->fetch('payload');
$first['first']->modify('+1 day');
$first['nested']['label'] = 'mutated-first';

$second = $cache->fetch('payload');
$secondDateUnchanged = $second['first']->format('Y-m-d H:i:s.u e') === '2026-07-03 12:34:56.123456 Asia/Tokyo';
$secondNestedUnchanged = $second['nested']['label'] === 'original';
$second['first']->modify('+2 days');
$second['nested']['label'] = 'mutated-second';

$third = $cache->fetch('payload');

echo "status: ", pcntl_wexitstatus($status), "\n";
echo "first alias: ";
var_dump($first['first'] === $first['second']);
echo "second alias: ";
var_dump($second['first'] === $second['second']);
echo "third alias: ";
var_dump($third['first'] === $third['second']);
echo "fetches independent: ";
var_dump($first['first'] !== $second['first'] && $second['first'] !== $third['first']);
echo "second unchanged by first: ";
var_dump($secondDateUnchanged);
var_dump($secondNestedUnchanged);
echo "third unchanged by second: ";
var_dump($third['first']->format('Y-m-d H:i:s.u e') === '2026-07-03 12:34:56.123456 Asia/Tokyo');
var_dump($third['nested']['label'] === 'original');
echo "object ids preserved as stored: ";
var_dump($first['first']->cachedSelfId() === $first['storedId']);
var_dump($second['first']->cachedSelfId() === $second['storedId']);
var_dump($third['first']->cachedSelfId() === $third['storedId']);

/* Fetched objects retain ordinary destructors that may re-enter the API across processes. */
class ReentrantDtor
{
    public int $n = 0;

    public function __destruct()
    {
        $cache = UserCache\Cache::getPool('slot-dtor');
        $cache->store('dtor-side', 'ran');
        $cache->fetch('dtor-side');
    }
}

$cache = UserCache\Cache::getPool('slot-dtor');
$cache->clear();

$obj = new ReentrantDtor();
$obj->n = 1;
var_dump($cache->store('obj', $obj));
unset($obj);

$first = $cache->fetch('obj');
var_dump($first->n);
unset($first);

$pid = pcntl_fork();
if ($pid === 0) {
    $child = new ReentrantDtor();
    $child->n = 2;
    UserCache\Cache::getPool('slot-dtor')->store('obj', $child);
    exit(0);
}

if ($pid > 0) {
    pcntl_waitpid($pid, $status);

    $second = $cache->fetch('obj');
    var_dump($second->n);
    var_dump($cache->fetch('dtor-side'));
} else {
    echo "pcntl_fork() failed\n";
}
?>
--EXPECT--
prototype fork pins:
parent pins: 2
cold pins: 3
bool(true)
one pins: 4
bool(true)
two pins: 5
bool(true)
bool(true)
remaining parent pins: 2

request-local slots:
bool(true)
status: 0
first alias: bool(true)
second alias: bool(true)
third alias: bool(true)
fetches independent: bool(true)
second unchanged by first: bool(true)
bool(true)
third unchanged by second: bool(true)
bool(true)
object ids preserved as stored: bool(true)
bool(true)
bool(true)
bool(true)
int(1)
int(2)
string(3) "ran"
