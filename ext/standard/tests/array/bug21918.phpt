--TEST--
Bug #21918 (different handling of positive vs. negative array indexes)
--FILE--
<?php

echo "==Mixed==\n";
$a = array(-1=>'a', '-2'=>'b', 3=>'c', '4'=>'d', 5=>'e', '6001'=>'f', '07'=>'g');

foreach($a as $k => $v) {
    var_dump($k);
    var_dump($v);
}

echo "==Normal==\n";
$b = array();
$b[] = 'a';

foreach($b as $k => $v) {
    var_dump($k);
    var_dump($v);
}

echo "==Negative==\n";
$c = array('-2' => 'a');

foreach($c as $k => $v) {
    var_dump($k);
    var_dump($v);
}

?>
--EXPECT--
==Mixed==
int(-1)
string(1) "a"
int(-2)
string(1) "b"
int(3)
string(1) "c"
int(4)
string(1) "d"
int(5)
string(1) "e"
int(6001)
string(1) "f"
string(2) "07"
string(1) "g"
==Normal==
int(0)
string(1) "a"
==Negative==
int(-2)
string(1) "a"
