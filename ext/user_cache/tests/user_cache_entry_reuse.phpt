--TEST--
UserCache\Cache: overwrites reuse entries and any value type can replace any other
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* Repeated overwrites and delete-store cycles of one key never fail. */
$cache = UserCache\Cache::getPool('entry-reuse-overwrite');

for ($i = 0; $i < 20000; $i++) {
    if (!$cache->store('same', ['i' => $i, 'payload' => str_repeat('x', 1024)])) {
        echo "same failed at $i\n";
        exit;
    }
}

$same = $cache->fetch('same');
var_dump($same['i']);
var_dump(strlen($same['payload']));

for ($i = 0; $i < 5000; $i++) {
    if (!$cache->delete('same')) {
        echo "delete failed at $i\n";
        exit;
    }
    if (!$cache->store('same', ['i' => $i])) {
        echo "store failed at $i\n";
        exit;
    }
}

$churned = $cache->fetch('same');
var_dump($churned['i']);

/* Scalar and payload entries replace each other without disturbing held values or accounting. */
$cache = UserCache\Cache::getPool('entry-reuse-transitions');
$values = [
    null, false, true, 0, -0.0, '', str_repeat("a\0b", 170),
    str_repeat('long', 2048), ['object' => (object) ['n' => 7]],
];

foreach ($values as $oldIndex => $old) {
    foreach ($values as $newIndex => $new) {
        $cache->clear();
        if (!$cache->store('value', $old)) {
            die("initial store failed: $oldIndex\n");
        }
        $held = $cache->fetch('value');
        if (!$cache->store('value', $new)) {
            die("replacement failed: $oldIndex/$newIndex\n");
        }
        for ($read = 0; $read < 3; $read++) {
            if (serialize($cache->fetch('value')) !== serialize($new)) {
                die("fetch mismatch: $oldIndex/$newIndex/$read\n");
            }
        }
        if (serialize($held) !== serialize($old)) {
            die("held value changed: $oldIndex/$newIndex\n");
        }
        $cache->store('epoch', 1);
        if (serialize($cache->fetch('value')) !== serialize($new)) {
            die("fetch after mutation mismatch: $oldIndex/$newIndex\n");
        }
        if (!$cache->delete('value') || $cache->has('value')) {
            die("delete failed: $oldIndex/$newIndex\n");
        }
    }
}
echo "transitions OK\n";

$strings = [];
foreach ([255, 256, 4095, 4096] as $length) {
    $key = "string-$length";
    $value = str_repeat('s', $length);
    if (!$cache->store($key, ['old' => [1, 2, 3]])
        || !$cache->store($key, $value) || $cache->fetch($key) !== $value) {
        die("string replacement failed: $length\n");
    }
    $strings[$key] = $value;
}
$cache->clear();
if (!$cache->storeMultiple($strings)) {
    die("bulk string store failed\n");
}
foreach ($strings as $key => $value) {
    if ($cache->fetch($key) !== $value) {
        die("bulk string mismatch: $key\n");
    }
}
echo "string boundaries OK\n";

$cache->clear();
$cache->store('scalar', 0);
$scalarMemory = $cache->getPoolStatus()->getUsedMemory();
foreach ([null, false, true, 0, 65536, PHP_INT_MIN, PHP_INT_MAX, 1.25, -0.0, INF, NAN] as $value) {
    $cache->store('scalar', $value);
    if ($cache->getPoolStatus()->getUsedMemory() !== $scalarMemory) {
        die("scalar memory depends on its bits\n");
    }
}
$cache->clear();
var_dump($cache->getPoolStatus()->getUsedMemory());
echo "accounting OK\n";
?>
--EXPECT--
int(19999)
int(1024)
int(4999)
transitions OK
string boundaries OK
int(0)
accounting OK
