--TEST--
UserCache\Cache: pool and status objects are equal only to themselves and take no dynamic properties
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
use UserCache\Cache;

$a = Cache::getPool('comparable-a');
$b = Cache::getPool('comparable-b');
var_dump($a == $b, $a == Cache::getPool('comparable-a'), in_array($b, [$a]), array_search($b, ['a' => $a, 'b' => $b]));

$empty = Cache::getStatus();
$a->store('k', 1);
var_dump($empty == Cache::getStatus(), $empty == $empty, $a->getPoolStatus() == $b->getPoolStatus());

try {
    $a->hook = 1;
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
string(1) "b"
bool(false)
bool(true)
bool(false)
Error: Cannot create dynamic property UserCache\Cache::$hook
