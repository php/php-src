--TEST--
Values fetched before pcntl_fork() stay pinned in the child after the parent releases them
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4M
--FILE--
<?php

$pool = UserCache\Cache::getPool('fork');
$expected = ['name' => str_repeat('A', 300), 'list' => range(1, 64), 'nested' => ['k' => str_repeat('B', 100)]];
$result = tempnam(sys_get_temp_dir(), 'ucache_fork_pins');

function same(array $cfg, array $expected): bool
{
    return $cfg['name'] === $expected['name'] && $cfg['list'] === $expected['list'] && $cfg['nested']['k'] === $expected['nested']['k'];
}

function waitFor(int $pid): string
{
    pcntl_waitpid($pid, $status);
    if (pcntl_wifsignaled($status)) {
        return 'signal ' . pcntl_wtermsig($status);
    }

    return 'exit ' . pcntl_wexitstatus($status);
}

echo "retained in child while the parent request is alive\n";
var_dump($pool->store('cfg', $expected));
$cfg = $pool->fetch('cfg');
$pinned = UserCache\Cache::getStatus()->getGraphPinnedReferences();
var_dump($pinned);

$child = pcntl_fork();
if ($child === 0) {
    $after = UserCache\Cache::getStatus()->getGraphPinnedReferences();
    $ok = $after === $pinned + 1;
    for ($i = 0; $i < 50 && $ok; $i++) {
        $pool->store('cfg', ['name' => str_repeat('C', 310 + $i), 'list' => range(1, 60), 'nested' => ['k' => 'x']]);
        $pool->store('filler' . ($i % 8), str_repeat('F', 500 + $i));
        $ok = same($cfg, $expected);
    }
    exit($ok ? 0 : 1);
}
echo "child ", waitFor($child), "\n";
var_dump(same($cfg, $expected));
var_dump($pool->store('cfg', $expected));

echo "retained by children forked back to back\n";
$children = [];
for ($i = 0; $i < 16; $i++) {
    $child = pcntl_fork();
    if ($child === 0) {
        usleep(20000);
        $ok = true;
        for ($j = 0; $j < 20 && $ok; $j++) {
            $ok = same($cfg, $expected);
            usleep(1000);
        }
        exit($ok ? 0 : 1);
    }
    $children[] = $child;
}
for ($i = 0; $i < 400; $i++) {
    $pool->store('cfg', ['name' => str_repeat('D', 300 + $i % 9), 'list' => range(1, 64), 'nested' => ['k' => 'y']]);
    $pool->store('filler' . ($i % 8), str_repeat('G', 400 + $i % 50));
}
$exits = [];
foreach ($children as $child) {
    $exits[] = waitFor($child);
}
var_dump(array_unique($exits));
var_dump($pool->store('cfg', $expected));

echo "retained in the grandchild after the parent request ended\n";
$parent = pcntl_fork();
if ($parent === 0) {
    $cfg = $pool->fetch('cfg');
    $grandchild = pcntl_fork();
    if ($grandchild === 0) {
        $ok = true;
        $until = microtime(true) + 1.5;
        while ($ok && microtime(true) < $until) {
            $ok = same($cfg, $expected);
            usleep(1000);
        }
        file_put_contents($GLOBALS['result'], $ok ? 'consistent' : 'changed');
        exit(0);
    }
    exit(0);
}
echo "parent ", waitFor($parent), "\n";
for ($i = 0; $i < 3000; $i++) {
    $pool->store('cfg', ['name' => str_repeat(chr(65 + $i % 26), 300 + $i % 7), 'list' => range(1, 64 + $i % 5), 'nested' => ['k' => str_repeat('Z', 100)]]);
    $pool->store('filler' . ($i % 40), str_repeat('F', 300 + $i % 100));
    if ($i % 500 === 0) {
        $pool->delete('cfg');
    }
}
for ($i = 0; $i < 200 && filesize($result) === 0; $i++) {
    usleep(50000);
    clearstatcache(true, $result);
}
echo "grandchild ", file_get_contents($result), "\n";
unlink($result);

?>
--EXPECT--
retained in child while the parent request is alive
bool(true)
int(1)
child exit 0
bool(true)
bool(true)
retained by children forked back to back
array(1) {
  [0]=>
  string(6) "exit 0"
}
bool(true)
retained in the grandchild after the parent request ended
parent exit 0
grandchild consistent
