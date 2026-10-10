--TEST--
UserCache\Cache: objects stored before their class gained __wakeup() or __unserialize() are restored through the hook the class has when fetched, like unserialize(), with numeric property keys and fetches re-entered from those hooks; a safe-direct subclass that gained its own __serialize()/__unserialize() is not restored
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
$cache = UserCache\Cache::getPool('gained-restore-hooks');
$cache->clear();

$pid = pcntl_fork();
if ($pid < 0) die('fork failed');
if ($pid === 0) {
    eval(<<<'PHP'
        class GainsWakeup { public $a = 1; protected $b = 2; private $c = 3; }
        class GainsUnserialize { public $a = 1; protected $b = 2; private $c = 3; }
        class GainsBoth { public $a = 1; }
        class SleepGainsUnserialize { public $a = 1; public $skipped = 2; public function __sleep(): array { return ['a']; } }
        class OverridingDate extends DateTimeImmutable {}
        class PlainDate extends DateTimeImmutable {}
    PHP);
    $values = [
        'wakeup' => new GainsWakeup(),
        'unserialize' => new GainsUnserialize(),
        'both' => new GainsBoth(),
        'sleep' => [new SleepGainsUnserialize(), new SleepGainsUnserialize(), new SleepGainsUnserialize()],
        'overriding-date' => new OverridingDate('2020-01-02 03:04:05 UTC'),
        'plain-date' => new PlainDate('2020-01-02 03:04:05 UTC'),
    ];
    $stored = true;
    foreach ($values as $key => $value) {
        $stored = $stored && $cache->store($key, $value) && $cache->store("native-$key", serialize($value));
    }
    exit($stored ? 0 : 1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status));

$log = [];
eval(<<<'PHP'
    class GainsWakeup {
        public $a; protected $b; private $c;
        public function __wakeup(): void { $GLOBALS['log'][] = ['wakeup', $this->a, $this->b, $this->c]; }
    }
    class GainsUnserialize {
        public $a; protected $b; private $c;
        public function __unserialize(array $data): void { $GLOBALS['log'][] = ['unserialize', array_map('bin2hex', array_keys($data)), array_values($data)]; }
    }
    class GainsBoth {
        public $a;
        public function __wakeup(): void { $GLOBALS['log'][] = ['wakeup']; }
        public function __unserialize(array $data): void { $GLOBALS['log'][] = ['unserialize', $data]; }
    }
    class SleepGainsUnserialize {
        public $a; public $skipped;
        public function __unserialize(array $data): void { $GLOBALS['log'][] = ['unserialize', $data]; }
    }
    class OverridingDate extends DateTimeImmutable {
        public function __serialize(): array { return []; }
        public function __unserialize(array $data): void { $GLOBALS['log'][] = ['date unserialize']; }
    }
    class PlainDate extends DateTimeImmutable {}
PHP);

foreach (['wakeup', 'unserialize', 'both', 'sleep'] as $key) {
    $log = [];
    $native = unserialize($cache->fetch("native-$key"));
    $native_log = $log;

    $log = [];
    $fetched = $cache->fetch($key);
    echo $key, ': ', $log === $native_log ? 'same hooks as unserialize()' : 'DIFFERENT: ' . json_encode([$log, $native_log]), "\n";
    var_dump($fetched == $native);
}

$log = [];
$cache->fetch('unserialize');
$cache->fetch('unserialize');
echo 'repeated fetches call __unserialize(): ', count($log), "\n";

$log = [];
var_dump($cache->fetch('overriding-date', 'NOT RESTORED'), $cache->has('overriding-date'), $log);
var_dump($cache->fetch('plain-date')->format(DATE_ATOM));

class Plain
{
    public $x = 1;
}

class Outer
{
    public $i;

    public function __sleep(): array
    {
        return ['i'];
    }

    public function __wakeup(): void
    {
        if ($this->i === 0) {
            echo "Outer::__wakeup fetches nested\n";
            $nested = UserCache\Cache::getPool('graph-restore-hook-frames')->fetch('nested');
            echo "nested fetched: ", get_class($nested[0]), ' ', $nested[0]->a, "\n";
        }
    }
}

function graph_restore_hook_frames(): void
{
    $cache = UserCache\Cache::getPool('graph-restore-hook-frames');
    $cache->clear();

    $pid = pcntl_fork();
    if ($pid === 0) {
        /* Previous deployment: SP has no restore hook and the plain object lets
         * the stored graph prefer request-local prototypes. */
        eval('class SP { public $a = 1; public function __serialize(): array { return ["a" => $this->a]; } }');
        $cache = UserCache\Cache::getPool('graph-restore-hook-frames');
        $cache->store('nested', [new SP(), new Plain()]);
        exit(0);
    }
    pcntl_waitpid($pid, $status);
    var_dump(pcntl_wexitstatus($status));

    /* Current deployment: SP gained __wakeup() without a cache reset. */
    eval('class SP { public $a = 1; public function __serialize(): array { return ["a" => $this->a]; } public function __wakeup(): void { echo "SP::__wakeup\n"; } }');

    /* Eight deferred hooks fill the outer queue, so a hook appended to it by the
     * nested fetch would move the entry the outer loop is running. */
    $outer = [];
    for ($i = 0; $i < 8; $i++) {
        $object = new Outer();
        $object->i = $i;
        $outer[] = $object;
    }
    var_dump($cache->store('outer', $outer));
    $fetched = $cache->fetch('outer');
    var_dump(count($fetched), $fetched[7]->i);
}

echo "\ngraph restore hook frames:\n";
graph_restore_hook_frames();

function graph_gained_unserialize_numeric_keys(): void
{
    $errorReporting = error_reporting(E_ALL & ~E_DEPRECATED);

    $cache = UserCache\Cache::getPool('gained-unserialize-numeric-keys');
    $cache->clear();

    $pid = pcntl_fork();
    if ($pid === 0) {
        eval('class WakeupState { public $a = 1; public function __wakeup(): void {} }');
        eval('class SleepState { public $a = 1; public $skipped = 2; public function __sleep(): array { return ["a", "7"]; } }');
        $wakeup = new WakeupState();
        $wakeup->{'0'} = 'zero';
        $wakeup->{'12'} = 'twelve';
        $sleep = new SleepState();
        $sleep->{'7'} = 'seven';
        $stored = $cache->store('wakeup', $wakeup) && $cache->store('native-wakeup', serialize($wakeup)) &&
            $cache->store('sleep', $sleep) && $cache->store('native-sleep', serialize($sleep));
        exit($stored ? 0 : 1);
    }
    pcntl_waitpid($pid, $status);
    var_dump(pcntl_wexitstatus($status));

    eval('class WakeupState { public $a; public array $data = []; public function __unserialize(array $data): void { $this->data = $data; } }');
    eval('class SleepState { public $a; public array $data = []; public function __unserialize(array $data): void { $this->data = $data; } }');

    foreach (['wakeup', 'sleep'] as $key) {
        $native = unserialize($cache->fetch("native-$key"))->data;
        $fetched = $cache->fetch($key)->data;
        echo $key, ': ', $fetched === $native ? 'same as unserialize()' : 'DIFFERENT', ' ', json_encode(array_map(gettype(...), array_keys($fetched))), "\n";
        var_dump($fetched[array_key_last($fetched)]);
    }

    error_reporting($errorReporting);
}

echo "\ngraph gained unserialize numeric keys:\n";
graph_gained_unserialize_numeric_keys();

function graph_gained_wakeup_leaks_before_prototype(): void
{
    $cache = UserCache\Cache::getPool('gained-wakeup-leaks');
    $cache->clear();

    $pid = pcntl_fork();
    if ($pid === 0) {
        eval('class LeakHolder { public $ao; public $p = 1; } class LeakingWakeup { public $sib; }');
        $holder = new LeakHolder();
        $holder->ao = new ArrayObject([new stdClass()]);
        $leaking = new LeakingWakeup();
        $leaking->sib = $holder;
        $objectBacked = new LeakHolder();
        $objectBacked->ao = @new ArrayObject(new stdClass());
        try {
            $cache->store('object-backed', ['holder' => $objectBacked]);
            exit(2);
        } catch (TypeError $e) {
            echo $e->getMessage(), "\n";
        }
        exit($cache->store('k', ['holder' => $holder, 'leaking' => $leaking]) ? 0 : 1);
    }
    pcntl_waitpid($pid, $status);
    var_dump(pcntl_wexitstatus($status));

    /* __wakeup() publishes a sibling that the request-local prototype clone would copy next. */
    eval('class LeakHolder { public $ao; public $p = 1; } class LeakingWakeup { public $sib; public function __wakeup(): void { $GLOBALS["leaked_holder"] = $this->sib; $GLOBALS["wakeups"]++; } }');
    $GLOBALS['leaked_holder'] = null;
    $GLOBALS['wakeups'] = 0;
    $GLOBALS['deprecations'] = 0;
    set_error_handler(function (int $errno): bool {
        if ($errno === E_DEPRECATED) {
            $GLOBALS['deprecations']++;
        }
        if ($GLOBALS['leaked_holder'] instanceof LeakHolder) {
            $GLOBALS['leaked_holder']->ao = null;
        }

        return true;
    });

    for ($i = 0; $i < 3; $i++) {
        $fetched = $cache->fetch('k');
    }

    /* An ArrayObject backed by an object was refused at store time, so no deprecation runs user code mid-fetch. */
    $unstored = $cache->fetch('object-backed', 'unstored');

    restore_error_handler();

    echo 'wakeups: ', $GLOBALS['wakeups'], "\n";
    var_dump($fetched['holder'] instanceof LeakHolder, $fetched['leaking']->sib === $fetched['holder']);
    var_dump($unstored, $GLOBALS['deprecations']);

    $GLOBALS['leaked_holder'] = null;
}

echo "\ngraph gained wakeup leaks before prototype:\n";
graph_gained_wakeup_leaks_before_prototype();
?>
--EXPECT--
int(0)
wakeup: same hooks as unserialize()
bool(true)
unserialize: same hooks as unserialize()
bool(true)
both: same hooks as unserialize()
bool(true)
sleep: same hooks as unserialize()
bool(true)
repeated fetches call __unserialize(): 2
string(12) "NOT RESTORED"
bool(false)
array(0) {
}
string(25) "2020-01-02T03:04:05+00:00"

graph restore hook frames:
int(0)
bool(true)
Outer::__wakeup fetches nested
SP::__wakeup
nested fetched: SP 1
int(8)
int(7)

graph gained unserialize numeric keys:
int(0)
wakeup: same as unserialize() ["string","integer","integer"]
string(6) "twelve"
sleep: same as unserialize() ["string","integer"]
string(5) "seven"

graph gained wakeup leaks before prototype:
The state of the ArrayObject object cannot be stored in the user cache
int(0)
wakeups: 3
bool(true)
bool(true)
string(8) "unstored"
int(0)
