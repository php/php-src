--TEST--
UserCache\Cache: fetchMultiple key handling, storeMultiple validation and rollback, and bulk operations snapshot referenced keys and values before userland runs
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
--FILE--
<?php
/* fetchMultiple normalizes numeric keys to integers. */
$cache = UserCache\Cache::getPool('multiple-int-keys');

var_dump($cache->storeMultiple([
    123 => 'int-key',
    '456' => 'numeric-string-key',
]));

$result = $cache->fetchMultiple([123, '456', 789], 'default');
var_dump($result);
var_dump(array_key_exists(123, $result));
var_dump(array_key_exists(456, $result));
var_dump($result[123], $result[456], $result[789]);

/* fetchMultiple keeps input key order when warm, cold and missing keys mix. */
class OrderProbe
{
    public function __construct(public int $n) {}
}

$cache = UserCache\Cache::getPool('multiple-order');
$cache->clear();

var_dump($cache->store('cold', new OrderProbe(1)));
var_dump($cache->store('warm', new OrderProbe(2)));

$cache->fetch('warm');

$result = $cache->fetchMultiple(['cold', 'warm', 'missing'], 'dflt');
var_dump(array_keys($result));
var_dump($result['cold']->n, $result['warm']->n, $result['missing']);

/* storeMultiple validates the whole batch before writing anything. */
$cache = UserCache\Cache::getPool('multiple-typeerror');

var_dump($cache->store('existing', 'previous'));

try {
    $cache->storeMultiple([
        'existing' => 'replaced',
        'fresh' => 1,
        'bad' => fn() => 1,
    ]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->fetch('existing'));
var_dump($cache->has('fresh'));

try {
    $cache->storeMultiple(['bad' => fopen('php://memory', 'r')]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* storeMultiple rolls back partial writes across every pair of value kinds. */
$cache = UserCache\Cache::getPool('multiple-rollback');

$values = [null, false, true, PHP_INT_MIN, -0.0, str_repeat('s', 300),
    str_repeat('g', 8192), ['n' => (object) ['x' => 1]]];
$tooLarge = str_repeat('x', 4 * 1024 * 1024);

for ($offset = 0; $offset < count($values); $offset++) {
    $cache->clear();
    $original = $replacement = [];
    foreach ($values as $index => $value) {
        $key = "value-$index";
        $original[$key] = $value;
        $replacement[$key] = $values[($index + $offset) % count($values)];
    }
    if (!$cache->storeMultiple($original) || !$cache->store('last', 7)) {
        die("initial store failed: $offset\n");
    }
    $used = $cache->getPoolStatus()->getUsedMemory();
    if ($cache->storeMultiple($replacement + ['fresh' => $values[$offset], 'last' => $tooLarge])) {
        die("bulk unexpectedly succeeded: $offset\n");
    }
    foreach ($original as $key => $value) {
        if (!$cache->has($key) || serialize($cache->fetch($key)) !== serialize($value)) {
            die("rollback mismatch: $offset/$key\n");
        }
    }
    if ($cache->fetch('last') !== 7 || $cache->has('fresh')) {
        die("rollback keys mismatch: $offset\n");
    }
    if ($cache->getPoolStatus()->getUsedMemory() !== $used) {
        die("rollback memory mismatch: $offset\n");
    }
    $replacement['fresh'] = $values[$offset];
    if (!$cache->storeMultiple($replacement)) {
        die("bulk reuse failed: $offset\n");
    }
    foreach ($replacement as $key => $value) {
        if (!$cache->has($key) || serialize($cache->fetch($key)) !== serialize($value)) {
            die("bulk reuse mismatch: $offset/$key\n");
        }
    }
}
echo "rollback OK\n";

$fresh = UserCache\Cache::getPool('multiple-rollback-fresh');
var_dump($fresh->storeMultiple([
    'fresh-a' => 'new-a',
    'fresh-b' => $tooLarge,
]));
var_dump($fresh->has('fresh-a'));
var_dump($fresh->has('fresh-b'));

echo "\nreference mutation:\n";

class RewritesKeyOnDestruct
{
    public function __destruct()
    {
        global $key;
        $key = $GLOBALS['replacement'];
    }
}

class RewritesKeyOnWakeup
{
    public $n = 1;

    public function __wakeup(): void
    {
        global $key;
        $key = new stdClass;
    }
}

class RewritesValueOnSerialize
{
    public function __serialize(): array
    {
        global $value;
        $value = $GLOBALS['replacement'];

        return ['x' => 1];
    }

    public function __unserialize(array $data): void
    {
    }
}

$replacement = null;
$cache = UserCache\Cache::getPool('reference-mutation');
$cache->store('destructs', new RewritesKeyOnDestruct);
$cache->store('wakes', new RewritesKeyOnWakeup);
$cache->store('plain', 'plain value');
$cache->store('7', 'seven');

foreach ([null, [1, 2, 3], 12345] as $replacement) {
    $key = 'plain';
    $result = $cache->fetchMultiple(['destructs', 'destructs', &$key]);
    var_dump(array_keys($result), $result['plain']);
}

$key = 7;
$result = $cache->fetchMultiple(['wakes', &$key]);
var_dump(array_keys($result), $result[7]);

foreach ([12345, 'short', str_repeat('b', 5000)] as $replacement) {
    $value = str_repeat('a', 300);
    var_dump($cache->storeMultiple(['referenced' => &$value, 'hook' => new RewritesValueOnSerialize]));
    var_dump(strlen($cache->fetch('referenced')), $cache->fetch('referenced')[0]);
}

$value = 'tiny';
$replacement = 1;
var_dump($cache->storeMultiple(['referenced' => &$value, 'hook' => new RewritesValueOnSerialize]));
var_dump($cache->fetch('referenced'));
?>
--EXPECT--
bool(true)
array(3) {
  [123]=>
  string(7) "int-key"
  [456]=>
  string(18) "numeric-string-key"
  [789]=>
  string(7) "default"
}
bool(true)
bool(true)
string(7) "int-key"
string(18) "numeric-string-key"
string(7) "default"
bool(true)
bool(true)
array(3) {
  [0]=>
  string(4) "cold"
  [1]=>
  string(4) "warm"
  [2]=>
  string(7) "missing"
}
int(1)
int(2)
string(4) "dflt"
bool(true)
Closure objects cannot be stored in the user cache
string(8) "previous"
bool(false)
Resources cannot be stored in the user cache
rollback OK
bool(false)
bool(false)
bool(false)

reference mutation:
array(2) {
  [0]=>
  string(9) "destructs"
  [1]=>
  string(5) "plain"
}
string(11) "plain value"
array(2) {
  [0]=>
  string(9) "destructs"
  [1]=>
  string(5) "plain"
}
string(11) "plain value"
array(2) {
  [0]=>
  string(9) "destructs"
  [1]=>
  string(5) "plain"
}
string(11) "plain value"
array(2) {
  [0]=>
  string(5) "wakes"
  [1]=>
  int(7)
}
string(5) "seven"
bool(true)
int(300)
string(1) "a"
bool(true)
int(300)
string(1) "a"
bool(true)
int(300)
string(1) "a"
bool(true)
string(4) "tiny"
