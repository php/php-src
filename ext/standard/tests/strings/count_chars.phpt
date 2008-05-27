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
--EXPECTF--
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
