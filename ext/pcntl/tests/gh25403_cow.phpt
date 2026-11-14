--TEST--
GH-25403: opting out of child memory re-keying avoids inherited free-list writes
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (PHP_OS !== 'Linux') die('skip Linux minor-fault accounting required');
if (getenv('USE_ZEND_ALLOC') === '0') die('skip Zend allocator required');
?>
--INI--
memory_limit=128M
--FILE--
<?php
$strings = [];
for ($i = 0; $i < 131072; $i++) {
    $strings[] = str_repeat('x', 256);
}
foreach ($strings as $i => $value) {
    if ($i % 2) unset($strings[$i]);
}
$pid = pcntl_fork(rekey_memory: false);
if ($pid === -1) throw new Exception('Fork failed');
if ($pid === 0) {
    var_dump(getrusage()['ru_minflt'] < 2048);
    exit(0);
}
if (pcntl_waitpid($pid, $status) !== $pid) throw new Exception('Wait failed');
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
?>
--EXPECT--
bool(true)
bool(true)
