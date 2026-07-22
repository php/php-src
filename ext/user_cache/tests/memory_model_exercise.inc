<?php

function user_cache_memory_model_active(): string
{
    ob_start();
    phpinfo(INFO_MODULES);
    $info = ob_get_clean();

    return preg_match('/^Active memory model => (.+)$/m', $info, $m) ? trim($m[1]) : 'unknown';
}

class MemoryModelPlain
{
    public int $a = 1;
    public ?string $b = null;
    public array $c = [];
}

class MemoryModelMagic
{
    public function __construct(public array $payload) {}

    public function __serialize(): array
    {
        return ['payload' => $this->payload];
    }

    public function __unserialize(array $data): void
    {
        $this->payload = $data['payload'];
    }
}

function user_cache_memory_model_exercise(string $expected): void
{
    $cache = UserCache\Cache::getPool('memory-model');
    $cache->clear();

    var_dump($cache->store('init', true));
    var_dump(UserCache\Cache::getStatus()->getAvailability()->name);
    $active = user_cache_memory_model_active();
    echo 'active model: ', $active === $expected ? 'as requested' : "MISMATCH ($active)", "\n";

    /* Scalars: entry-inline types and strings. */
    var_dump($cache->store('int', 42), $cache->fetch('int'));
    var_dump($cache->store('float', 1.5), $cache->fetch('float'));
    var_dump($cache->store('bool', false), $cache->fetch('bool'));
    var_dump($cache->store('null', null), $cache->fetch('null', 'miss'));
    var_dump($cache->store('str', 'hello'), $cache->fetch('str'));

    /* Arrays: packed, hashed, nested, empty. */
    $array = ['packed' => [1, 2, 3], 'map' => ['k' => 'v'], 'nested' => [['deep' => true]], 'empty' => []];
    var_dump($cache->store('array', $array));
    var_dump($cache->fetch('array') === $array);

    /* Objects: plain graph route and magic serialize route. */
    $plain = new MemoryModelPlain();
    $plain->b = 'x';
    $plain->c = [1];
    var_dump($cache->store('plain', $plain));
    $fetched = $cache->fetch('plain');
    var_dump($fetched instanceof MemoryModelPlain, $fetched->a, $fetched->b, $fetched->c);

    $magic = new MemoryModelMagic(['m' => 2]);
    var_dump($cache->store('magic', $magic));
    $fetchedMagic = $cache->fetch('magic');
    var_dump($fetchedMagic instanceof MemoryModelMagic, $fetchedMagic->payload);

    /* add / has / delete. */
    var_dump($cache->add('int', 99));
    var_dump($cache->add('added', 7), $cache->fetch('added'));
    var_dump($cache->delete('added'), $cache->has('added'));

    /* increment / decrement. */
    var_dump($cache->store('counter', 10));
    var_dump($cache->increment('counter', 5));
    var_dump($cache->decrement('counter'));

    /* Bulk operations. */
    var_dump($cache->storeMultiple(['m1' => 'a', 'm2' => 'b'], 60));
    var_dump($cache->fetchMultiple(['m1', 'm2', 'm3'], 'dflt'));
    var_dump($cache->deleteMultiple(['m1', 'm2']));

    /* lock / unlock / remember. */
    var_dump($cache->lock('lk'));
    var_dump($cache->unlock('lk'));
    $calls = 0;
    var_dump($cache->remember('rem', function () use (&$calls) {
        $calls++;
        return 'computed';
    }));
    var_dump($cache->remember('rem', function () use (&$calls) {
        $calls++;
        return 'recomputed';
    }));
    var_dump($calls);

    /* Cross-process visibility over fork. */
    $pid = pcntl_fork();
    if ($pid === 0) {
        $child = UserCache\Cache::getPool('memory-model');
        $ok = $child->fetch('str') === 'hello'
            && $child->fetch('array') === $array
            && $child->fetch('counter') === 14;
        echo $ok ? "child sees parent data\n" : "child MISSING parent data\n";
        var_dump($child->store('from-child', 'child-value'));
        var_dump($child->increment('counter', 100));
        exit(0);
    }

    if ($pid < 0) {
        echo "pcntl_fork() failed\n";
        return;
    }

    pcntl_waitpid($pid, $status);
    var_dump($cache->fetch('from-child'));
    var_dump($cache->fetch('counter'));

    var_dump($cache->clear());
    var_dump($cache->has('str'));
}
