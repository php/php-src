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
string(12) "*4*string* *"
string(12) "*4*stri0g* *"
string(12) "*-*-tri0g* *"
string(33) "*-**-*-tstringi0g* *tstringi0g* *"
string(33) "4-4*4*-tstringi0g* *tstringi0g* *"
string(33) "4-4*4*5t5tringi0g* *tstringi0g* *"
string(33) "454*4*5t5tringi0g* *tstringi0g* *"
string(1) "-"
string(33) "*-**-*-tstringi0g* *tstringi0g* *"
string(1) "4"
string(1) "5"
string(1) "5"
string(33) "a54*4*5t5tringi0g* *tstringi0a* a"
