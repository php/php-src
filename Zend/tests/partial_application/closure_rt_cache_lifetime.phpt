--TEST--
Partial application over a Closure::fromCallable() must not double-free the run_time_cache
--FILE--
<?php
class C {
    public function m($x, $y) { return $x + $y; }
}

$cl = Closure::fromCallable([new C, 'm']);
$partial = $cl(10, ?);
var_dump($partial(5));
unset($partial);
unset($cl);
gc_collect_cycles();
echo "OK\n";
?>
--EXPECT--
int(15)
OK
