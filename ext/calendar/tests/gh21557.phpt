--TEST--
GH-21557 jewishtojd returns 0 for years >= 6000
--CREDITS--
oleibman
--EXTENSIONS--
calendar
--FILE--
<?php
for ($yh = 5995; $yh < 6005; ++$yh) {
    $rh = jewishtojd(1, 1, $yh);
    echo "yh=$yh rh=$rh\n";
}
?>
--EXPECT--
yh=5995 rh=2537279
yh=5996 rh=2537633
yh=5997 rh=2538016
yh=5998 rh=2538371
yh=5999 rh=2538725
yh=6000 rh=2539110
yh=6001 rh=2539463
yh=6002 rh=2539818
yh=6003 rh=2540202
yh=6004 rh=2540557
