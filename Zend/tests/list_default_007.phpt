--TEST--
Testing list() default value in foreach context
--FILE--
<?php


$arr = [
	[],
	[1],
	[1, 2],
	[1, 2, 3]
];

foreach($arr as $key => list($one, $two ='default')) {
	echo "$key => $one, $two\n";
}

echo "-----------\n";

foreach($arr as $key => list($one='ONE')) {
	echo "$key => $one\n";
}

echo "-----------\n";

foreach($arr as $key => list($one=strlen('ONE'))) {
	echo "$key => $one\n";
}

?>
--EXPECTF--
Notice: Undefined offset: %d in %s on line %d
0 => , default
1 => 1, default
2 => 1, 2
3 => 1, 2
-----------
0 => ONE
1 => 1
2 => 1
3 => 1
-----------
0 => 3
1 => 1
2 => 1
3 => 1
