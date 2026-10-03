--TEST--
UserCache\Cache: shared-graph pins held by other processes survive deletePool(), orphan deleted payloads and are reclaimed after exit or SIGKILL
--EXTENSIONS--
pcntl
posix
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
--FILE--
<?php
/* Pins of a SIGKILLed worker are stripped by a later request and their payloads reclaimed. */
const KEYS = 40;
const WAIT_NS = 30000000000;

$cache = UserCache\Cache::getPool('dead-pin');
$cache->clear();

function free_memory(): int {
    return UserCache\Cache::getStatus()->getFreeMemory();
}

function seed_value(int $i): array {
    return ['i' => $i, 'pad' => str_repeat('x', 2000)];
}

function seed(UserCache\Cache $cache): void {
    for ($i = 0; $i < KEYS; $i++) {
        $cache->store("g:$i", seed_value($i));
    }
}

function forget(UserCache\Cache $cache): void {
    for ($i = 0; $i < KEYS; $i++) {
        $cache->delete("g:$i");
    }
}

/* Waits for another process to write $path; gives up when the child $pid exits first or the deadline passes. */
function wait_for_file(string $path, ?int $pid = null): bool {
    $deadline = hrtime(true) + WAIT_NS;
    do {
        clearstatcache();
        if (filesize($path) !== 0) {
            return true;
        }
        if ($pid !== null && pcntl_waitpid($pid, $status, WNOHANG) !== 0) {
            return false;
        }
        usleep(10000);
    } while (hrtime(true) < $deadline);

    return false;
}

/* Reaps $pid, killing it once the deadline passes, and reports whether it exited with status 0. */
function reap(int $pid): bool {
    $deadline = hrtime(true) + WAIT_NS;
    do {
        $ret = pcntl_waitpid($pid, $status, WNOHANG);
        if ($ret !== 0) {
            return $ret === $pid && pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0;
        }
        usleep(10000);
    } while (hrtime(true) < $deadline);
    posix_kill($pid, SIGKILL);
    pcntl_waitpid($pid, $status);

    return false;
}

function run_pinner(UserCache\Cache $cache, string $mode): bool {
    $ready = tempnam(sys_get_temp_dir(), 'uc_pin');
    $pid = pcntl_fork();
    if ($pid < 0) die("fork failed\n");
    if ($pid === 0) {
        for ($i = 0; $i < KEYS; $i++) {
            if ($cache->fetch("g:$i") !== seed_value($i)) exit(1);
        }
        file_put_contents($ready, 'r');
        if ($mode === 'kill') {
            sleep(30); /* SIGKILL window */
        }
        exit(0);
    }
    $pinned = wait_for_file($ready, $pid);
    @unlink($ready);
    if ($mode === 'kill' && $pinned) {
        posix_kill($pid, SIGKILL);
        pcntl_waitpid($pid, $status);

        return pcntl_wifsignaled($status) && pcntl_wtermsig($status) === SIGKILL;
    }

    return reap($pid) && $pinned;
}

$baseline = free_memory();
$status = UserCache\Cache::getStatus();
$owners0 = $status->getDeadPinOwnersReclaimed();
$stripped0 = $status->getDeadPinsStripped();

seed($cache);
var_dump(run_pinner($cache, 'clean'));
forget($cache);
var_dump(free_memory() === $baseline);

seed($cache);
var_dump(run_pinner($cache, 'kill'));
var_dump(run_pinner($cache, 'clean')); /* Run the request-end sweep. */
forget($cache);
var_dump(free_memory() === $baseline);

$status = UserCache\Cache::getStatus();
var_dump($status->getDeadPinOwnersReclaimed() - $owners0);
var_dump($status->getDeadPinsStripped() - $stripped0 === KEYS);

var_dump($status->getGraphPinnedReferences());

/* A child's pin keeps a payload alive across the parent's deletePool(). */
$cache = UserCache\Cache::getPool('del-pin');
$original = ['list' => range(1, 64), 'label' => str_repeat('x', 100)];
$cache->store('graph', $original);

$ready = tempnam(sys_get_temp_dir(), 'uc_dp_ready');
$deleted = tempnam(sys_get_temp_dir(), 'uc_dp_done');

$pid = pcntl_fork();
if ($pid < 0) die("fork failed\n");
if ($pid === 0) {
    $held = $cache->fetch('graph');
    file_put_contents($ready, 'r');

    if (!wait_for_file($deleted)) exit(2);

    $intact = $held === ['list' => range(1, 64), 'label' => str_repeat('x', 100)];
    echo "child: ", $intact ? "intact\n" : "CORRUPTED\n";
    exit($intact ? 0 : 1);
}

if (!wait_for_file($ready, $pid)) die("child did not pin the payload\n");

var_dump(UserCache\Cache::deletePool('del-pin'));
file_put_contents($deleted, 'd');
var_dump(reap($pid));

var_dump(UserCache\Cache::hasPool('del-pin'));
var_dump(UserCache\Cache::getPool('del-pin')->fetch('graph', 'MISS'));

@unlink($ready);
@unlink($deleted);

/* Payloads deleted or replaced while another process pins them are orphaned and freed when that process exits. */
$cache = UserCache\Cache::getPool('orphan-pin');
$cache->clear();
$cache->store('tick', 0);
for ($i = 20; $i < 40; $i++) {
    $cache->store("o:$i", $i);
}
$baseline = free_memory();

$pad = str_repeat('o', 3000);
for ($i = 0; $i < 40; $i++) {
    $cache->store("o:$i", ['i' => $i, 'pad' => $pad]);
}

$ready = tempnam(sys_get_temp_dir(), 'uc_orphan_ready');
$release = tempnam(sys_get_temp_dir(), 'uc_orphan_release');

$pid = pcntl_fork();
if ($pid < 0) die("fork failed\n");
if ($pid === 0) {
    $held = [];
    for ($i = 0; $i < 40; $i++) {
        $held[] = $cache->fetch("o:$i");
    }
    file_put_contents($ready, 'r');
    if (!wait_for_file($release)) exit(2);

    $intact = true;
    foreach ($held as $i => $value) {
        $intact = $intact && $value === ['i' => $i, 'pad' => $pad];
    }
    echo "child: ", $intact ? "intact\n" : "CORRUPTED\n";
    exit($intact ? 0 : 1);
}

if (!wait_for_file($ready, $pid)) die("child did not pin the payloads\n");

for ($i = 0; $i < 20; $i++) {
    $cache->delete("o:$i");
}
for ($i = 20; $i < 40; $i++) {
    $cache->store("o:$i", $i);
}
echo "held while pinned: ";
var_dump(free_memory() < $baseline);
var_dump(UserCache\Cache::getStatus()->getGraphPinnedReferences() > 0);

file_put_contents($release, 'd');
var_dump(reap($pid));

echo "freed by the child's request shutdown: ";
var_dump(free_memory() === $baseline);
var_dump(UserCache\Cache::getStatus()->getGraphPinnedReferences());

@unlink($ready);
@unlink($release);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
bool(true)
int(0)
bool(true)
child: intact
bool(true)
bool(false)
string(4) "MISS"
held while pinned: bool(true)
bool(true)
child: intact
bool(true)
freed by the child's request shutdown: bool(true)
int(0)
