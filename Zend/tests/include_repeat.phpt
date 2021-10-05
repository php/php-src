--TEST--
Repeated include must not increase memory
--FILE--
<?php

$m = -1;
$mDiff = -1;
$mPrev = 0;
for ($i = 0; $i < 10 * 1000; $i++) {
    require __DIR__ . '/include_repeat.inc';
    $m = memory_get_usage();
    $mDiff = $m - $mPrev;
    if ($mPrev !== 0 && $mDiff !== 0) {
        echo 'Increased memory detected: ' . $mDiff . ' B (' . $i . ' iteration)' . "\n";
        exit(1);
    }
    $mPrev = $m;
}
echo 'done';

?>
--EXPECT--
done
