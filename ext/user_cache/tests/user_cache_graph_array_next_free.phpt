--TEST--
UserCache\Cache: empty arrays keep their next free index in verbatim, dynamic, root and __wakeup() state encodings
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
class NextFreeWakeup
{
    public $arrays;

    public function __wakeup(): void
    {
    }
}

function probe(array $array): string
{
    $array[-5] = 'negative';
    $array[] = 'append';

    return json_encode(array_keys($array));
}

$emptied = [1, 2];
unset($emptied[0], $emptied[1]);

$zero = [];
$zero[-1] = 1;
unset($zero[-1]);

$fresh = array_filter([1], static fn () => false);

$separated = $emptied;
$separated['k'] = 1;
unset($separated['k']);

$cases = ['emptied' => $emptied, 'zero' => $zero, 'fresh' => $fresh, 'separated' => $separated, 'literal' => []];

$cache = UserCache\Cache::getPool('graph-array-next-free');
$object = new stdClass();
$wakeup = new NextFreeWakeup();
$wakeup->arrays = $cases;

$cache->store('verbatim', $cases);
$cache->store('dynamic', $cases + ['object' => $object]);
$cache->store('wakeup', $wakeup);
foreach ($cases as $name => $array) {
    $cache->store("root:$name", $array);
}

foreach ($cases as $name => $array) {
    $expected = probe($array);
    $results = [
        'verbatim' => probe($cache->fetch('verbatim')[$name]),
        'dynamic' => probe($cache->fetch('dynamic')[$name]),
        'wakeup' => probe($cache->fetch('wakeup')->arrays[$name]),
        'root' => probe($cache->fetch("root:$name")),
    ];
    echo $name, ' ', $expected, ' ', count(array_unique($results)) === 1 && reset($results) === $expected ? 'same' : json_encode($results), "\n";
}
?>
--EXPECT--
emptied [-5,2] same
zero [-5,0] same
fresh [-5,-4] same
separated [-5,2] same
literal [-5,-4] same
