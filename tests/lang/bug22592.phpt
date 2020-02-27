--TEST--
Bug #22592 (cascading assignments to strings with curly braces broken)
--FILE--
<?php
function error_hdlr($errno, $errstr) {
    echo "[$errstr]\n";
}

set_error_handler('error_hdlr');

$i = 4;
$s = "string";

$result = "* *-*";
var_dump($result);
$result[6] = '*';
var_dump($result);
$result[1] = $i;
var_dump($result);
$result[3] = $s;
var_dump($result);
$result[7] = 0;
var_dump($result);
$a = $result[1] = $result[3] = '-';
var_dump($result);
$b = $result[3] = $result[5] = $s;
var_dump($result);
$c = $result[0] = $result[2] = $result[4] = $i;
var_dump($result);
$d = $result[6] = $result[8] = 5;
var_dump($result);
$e = $result[1] = $result[6];
var_dump($result);
var_dump($a, $b, $c, $d, $e);
$result[0] = $result[-4] = $result[-1] = 'a';
var_dump($result);
?>
--EXPECT--
string(5) "* *-*"
string(7) "* *-* *"
string(7) "*4*-* *"
[Only the first byte will be assigned to the string offset]
string(7) "*4*s* *"
string(8) "*4*s* *0"
string(8) "*-*-* *0"
[Only the first byte will be assigned to the string offset]
string(8) "*-*s*s*0"
string(8) "4-4s4s*0"
string(9) "4-4s4s505"
string(9) "454s4s505"
string(1) "-"
string(1) "s"
string(1) "4"
string(1) "5"
string(1) "5"
string(9) "a54s4a50a"
