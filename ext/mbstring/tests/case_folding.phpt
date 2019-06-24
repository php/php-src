--TEST--
Test full case folding in case-insensitive operations
--FILE--
<?php

$str1 = "ß SS ß";
$str2 = "ß";
$len = mb_strlen($str1);

for ($i = -$len; $i <= $len; $i++) {
    echo "stripos($str1, $str2, $i): " . mb_stripos($str1, $str2, $i) . "\n"; 
}

for ($i = -$len; $i <= $len; $i++) {
    echo "strripos($str1, $str2, $i): " . mb_strripos($str1, $str2, $i) . "\n"; 
}

echo "\nTurkish I:\n";
$str1 = "İyi akşamlar";
$str2 = "i̇yi"; // with combining dot above
$str3 = "iyi"; // without

var_dump(mb_stripos($str1, $str2));
var_dump(mb_stripos($str1, $str3));

?>
--EXPECT--
stripos(ß SS ß, ß, -6): 0
stripos(ß SS ß, ß, -5): 2
stripos(ß SS ß, ß, -4): 2
stripos(ß SS ß, ß, -3): 5
stripos(ß SS ß, ß, -2): 5
stripos(ß SS ß, ß, -1): 5
stripos(ß SS ß, ß, 0): 0
stripos(ß SS ß, ß, 1): 2
stripos(ß SS ß, ß, 2): 2
stripos(ß SS ß, ß, 3): 5
stripos(ß SS ß, ß, 4): 5
stripos(ß SS ß, ß, 5): 5
stripos(ß SS ß, ß, 6): 
strripos(ß SS ß, ß, -6): 0
strripos(ß SS ß, ß, -5): 0
strripos(ß SS ß, ß, -4): 2
strripos(ß SS ß, ß, -3): 2
strripos(ß SS ß, ß, -2): 2
strripos(ß SS ß, ß, -1): 5
strripos(ß SS ß, ß, 0): 5
strripos(ß SS ß, ß, 1): 5
strripos(ß SS ß, ß, 2): 5
strripos(ß SS ß, ß, 3): 5
strripos(ß SS ß, ß, 4): 5
strripos(ß SS ß, ß, 5): 5
strripos(ß SS ß, ß, 6): 

Turkish I:
int(0)
bool(false)
