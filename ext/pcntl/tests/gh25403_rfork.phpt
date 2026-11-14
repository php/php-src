--TEST--
pcntl_rfork() preserves child initialization with optional memory re-keying
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_rfork')) die('skip pcntl_rfork unavailable');
?>
--FILE--
<?php
function checkChild(bool $rekey): void {
    $objects = [];
    for ($i = 0; $i < 8192; $i++) {
        $objects[] = str_repeat('x', 128 + $i % 64);
    }
    foreach ($objects as $i => $value) {
        if ($i % 2) unset($objects[$i]);
    }
    realpath(__FILE__);
    $pid = pcntl_rfork(0, 0, rekey_memory: $rekey);
    if ($pid === -1) throw new Exception('Fork failed');
    if ($pid === 0) {
        if (array_key_exists(__FILE__, realpath_cache_get())) exit(1);
        for ($i = 0; $i < 8192; $i++) {
            $objects[] = str_repeat('y', 128 + $i % 64);
        }
        gc_mem_caches();
        $pid = pcntl_fork(rekey_memory: !$rekey);
        if ($pid === -1) exit(1);
        if ($pid === 0) {
            unset($objects);
            gc_mem_caches();
            exit(0);
        }
        if (pcntl_waitpid($pid, $status) !== $pid) exit(1);
        exit(pcntl_wifexited($status) ? pcntl_wexitstatus($status) : 1);
    }
    var_dump(pcntl_waitpid($pid, $status) === $pid);
    var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
}
checkChild(false);
checkChild(true);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
