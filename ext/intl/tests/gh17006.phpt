--TEST--
NumberFormatter::format using new API
--EXTENSIONS--
intl
--FILE--
<?php
$number = 1_000_000;
$oldf = new \NumberFormatter('en_US', \NumberFormatter::SPELLOUT);
$oldv = $oldf->format($number);
var_dump($oldv);
$newf = new \NumberFormatter('en_US', "compact-long");
$newv = $newf->format($number);
var_dump($newv);
?>
--EXPECT--
string(11) "one million"
string(9) "1 million"
