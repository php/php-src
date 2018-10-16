--TEST--
Hash: tiger algorithm
--FILE--
<?php
echo hash('tiger192,3', ''),"\n";
echo hash('tiger192,3', 'abc'),"\n";
echo hash('tiger192,3', str_repeat('a', 63)),"\n";
echo hash('tiger192,3', str_repeat('abc', 61)),"\n";
echo hash('tiger192,3', str_repeat('abc', 64)),"\n";
?>
--EXPECT--
3293ac630c13f0245f92bbb1766e16167a4e58492dde73f3
2aab1484e8c158f2bfb8c5ff41b57a525129131c957b5f93
9366604ea109e48ed763caabb2d5633b4946eb295ef5781a
b19abf166d158625808f035edf8be4e6b0bcb31d070ec353
badd965340a9e83e4a16f48a5038c01b856a9158ef59fec1
