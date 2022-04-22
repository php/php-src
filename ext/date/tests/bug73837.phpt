--TEST--
Bug #73837: Milliseconds in DateTime()
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
$startTS = time();
$prev_dt = new DateTime();
while (time() < $startTS + 2) {
    $dt = new DateTime();
    if ($prev_dt > $dt) {
        var_dump($prev_dt->format("Y-m-d H:i:s.u"));
        var_dump($dt->format("Y-m-d H:i:s.u"));
        break;
    }
    $prev_dt = $dt;
}
echo "Finished\n";
?>
--EXPECTF--
Finished
