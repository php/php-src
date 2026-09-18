--TEST--
UserCache\Cache: equal-content pure-data sub-arrays share one verbatim copy
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('verbatim-dedup');
$usedMemory = static fn (): int => UserCache\Cache::getStatus()->getUsedMemory();

$row = ['controller' => 'CatalogController::show', 'methods' => ['GET'], 'flags' => [true, false, true], 'score' => 100];
$identicalRows = [];
$distinctRows = [];
for ($i = 0; $i < 128; $i++) {
    $identicalRows[] = $row;
    $distinctRows[] = ['controller' => 'CatalogController::show', 'methods' => ['GET'], 'flags' => [true, false, true], 'score' => $i];
}

$cost = static function (string $key, array $rows) use ($cache, $usedMemory): int {
    $cache->clear();
    $before = $usedMemory();
    var_dump($cache->store($key, $rows));

    return $usedMemory() - $before;
};

$identicalCost64 = $cost('identical', array_slice($identicalRows, 0, 64));
$identicalCost128 = $cost('identical', $identicalRows);
$distinctCost64 = $cost('distinct', array_slice($distinctRows, 0, 64));
$distinctCost128 = $cost('distinct', $distinctRows);

/* Compare what the second 64 rows cost: the entry, key and root-array
 * overhead is paid either way and its size depends on the platform, so it
 * must not take part in the ratio. */
var_dump(($identicalCost128 - $identicalCost64) * 8 < ($distinctCost128 - $distinctCost64));

$cache->clear();
var_dump($cache->store('identical', $identicalRows));
$fetched = $cache->fetch('identical');
var_dump($fetched === $identicalRows);

$fetched[3]['score'] = 999;
$fetched[3]['methods'][] = 'POST';
var_dump($fetched[4]['score'], count($fetched[4]['methods']));
var_dump($cache->fetch('identical')[3]['score']);

$sparse = [1, 2];
unset($sparse[1]);
var_dump($cache->store('next-free', ['sparse' => $sparse, 'dense' => [1]]));
$fetched = $cache->fetch('next-free');
$fetched['sparse'][] = 'x';
$fetched['dense'][] = 'y';
var_dump(array_keys($fetched['sparse']), array_keys($fetched['dense']));

var_dump($cache->store('signed-zero', [[-0.0], [0.0]]));
var_dump($cache->fetch('signed-zero'));

var_dump($cache->store('object-root', [new stdClass(), $row, $row]));
$fetched = $cache->fetch('object-root');
var_dump($fetched[1] === $row, $fetched[2] === $row);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(100)
int(1)
int(100)
bool(true)
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(2)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
bool(true)
array(2) {
  [0]=>
  array(1) {
    [0]=>
    float(-0)
  }
  [1]=>
  array(1) {
    [0]=>
    float(0)
  }
}
bool(true)
bool(true)
bool(true)
