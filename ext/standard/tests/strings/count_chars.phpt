--TEST--
count_chars() function
--FILE--
<?php
$s = "het leven is net erwtensoep - je kunt er geen touw aan vastknopen";
for($i=0; $i<3; $i++) {
	$c = count_chars($s, $i);
	if ($i == 1) {
		print_r($c);
	} else {
		echo implode($c),"\n";
	}
}

echo $a = count_chars($s, 3), "\n";
var_dump((int) strlen(count_chars($s, 4)) == 256-strlen($a));

?>
--EXPECT--
000000000000000000000000000000001200000000000010000000000000000000000000000000000000000000000000003000120111121083202362220000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
Array
(
    [32] => 12
    [45] => 1
    [97] => 3
    [101] => 12
    [103] => 1
    [104] => 1
    [105] => 1
    [106] => 1
    [107] => 2
    [108] => 1
    [110] => 8
    [111] => 3
    [112] => 2
    [114] => 2
    [115] => 3
    [116] => 6
    [117] => 2
    [118] => 2
    [119] => 2
)
000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 -aeghijklnoprstuvw
bool(true)
--UEXPECTF--
Warning: count_chars(): Only mode=1 is supported with Unicode strings in %s on line %d

Warning: implode(): Argument must be an array in %s on line %d

Array
(
    [h] => 1
    [e] => 12
    [t] => 6
    [ ] => 12
    [l] => 1
    [v] => 2
    [n] => 8
    [i] => 1
    [s] => 3
    [r] => 2
    [w] => 2
    [o] => 3
    [p] => 2
    [-] => 1
    [j] => 1
    [k] => 2
    [u] => 2
    [g] => 1
    [a] => 3
)

Warning: count_chars(): Only mode=1 is supported with Unicode strings in %s on line %d

Warning: implode(): Argument must be an array in %s on line %d


Warning: count_chars(): Only mode=1 is supported with Unicode strings in %s on line %d


Warning: count_chars(): Only mode=1 is supported with Unicode strings in %s on line %d
bool(false)
