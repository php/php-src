--TEST--
UserCache\Cache: forked child does not inherit parent entry lock ownership
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('fork-entry-lock');
$cache->clear();

$shared = range(1, 4);
$value = [$shared, $shared, ['nested' => $shared]];

var_dump($cache->store('graph', $value));
var_dump($cache->lock('lock'));

$pid = pcntl_fork();
if ($pid === 0) {
    echo "child unlock\n";
    var_dump($cache->unlock('lock'));
    echo "child lock\n";
    var_dump($cache->lock('lock', 1));
    echo "child fetch\n";
    var_dump($cache->fetch('graph'));
    exit(0);
}

if ($pid > 0) {
    pcntl_waitpid($pid, $status);
    echo "parent unlock\n";
    var_dump($cache->unlock('lock'));
} else {
    echo "pcntl_fork() failed\n";
}
?>
--EXPECT--
bool(true)
bool(true)
child unlock
bool(false)
child lock
bool(false)
child fetch
array(3) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
  [1]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
  [2]=>
  array(1) {
    ["nested"]=>
    array(4) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
    }
  }
}
parent unlock
bool(true)
