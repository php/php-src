--TEST--
idate() function: ISO Day-of-Week and ISO Year
--FILE--
<?php
date_default_timezone_set('UTC');
$tmp = "UwNW";
for ($a = 0; $a < strlen($tmp); $a++){
    echo $tmp[$a], ': ', idate($tmp[$a], 1041808859), "\n";
}
$tmp = "UYoW";
for ($a = 0; $a < strlen($tmp); $a++){
    echo $tmp[$a], ': ', idate($tmp[$a], 1072912859), "\n";
}
?>
--EXPECT--
U: 1041808859
w: 0
N: 7
W: 1
U: 1072912859
Y: 2003
o: 2004
W: 1
