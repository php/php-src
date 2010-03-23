--TEST--
Test base_convert() - basic function tests base_convert()
--FILE--
<?php
$frombase = array(2,8,10,16,36);
$tobase = array(2,8,10,16,36);

$values = array(10,
				27,
				39,
				039,
				0x5F,	
				"10",
				"27",
				"39",
				"5F", 
				"3XYZ"
				);	

for ($f= 0; $f < count($frombase); $f++) {
	echo "\n...from base is ", $frombase[$f], "\n";
	for ($t= 0; $t < count($tobase); $t++) {
		echo "......to base is ", $tobase[$t], "\n";	
		for ($i =0; $i < count($values); $i++){ 	
			$res = base_convert($values[$i],$frombase[$f],$tobase[$t]);
			echo ".........value= ", $values[$i], " res = ", $res, "\n";
		}		
	}	
}
?>

--EXPECTF--
...from base is 2
......to base is 2
.........value= 10 res = 10
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 3 res = 0
.........value= 95 res = 0
.........value= 10 res = 10
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 5F res = 0
.........value= 3XYZ res = 0
......to base is 8
.........value= 10 res = 2
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 3 res = 0
.........value= 95 res = 0
.........value= 10 res = 2
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 5F res = 0
.........value= 3XYZ res = 0
......to base is 10
.........value= 10 res = 2
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 3 res = 0
.........value= 95 res = 0
.........value= 10 res = 2
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 5F res = 0
.........value= 3XYZ res = 0
......to base is 16
.........value= 10 res = 2
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 3 res = 0
.........value= 95 res = 0
.........value= 10 res = 2
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 5F res = 0
.........value= 3XYZ res = 0
......to base is 36
.........value= 10 res = 2
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 3 res = 0
.........value= 95 res = 0
.........value= 10 res = 2
.........value= 27 res = 0
.........value= 39 res = 0
.........value= 5F res = 0
.........value= 3XYZ res = 0

...from base is 8
......to base is 2
.........value= 10 res = 1000
.........value= 27 res = 10111
.........value= 39 res = 11
.........value= 3 res = 11
.........value= 95 res = 101
.........value= 10 res = 1000
.........value= 27 res = 10111
.........value= 39 res = 11
.........value= 5F res = 101
.........value= 3XYZ res = 11
......to base is 8
.........value= 10 res = 10
.........value= 27 res = 27
.........value= 39 res = 3
.........value= 3 res = 3
.........value= 95 res = 5
.........value= 10 res = 10
.........value= 27 res = 27
.........value= 39 res = 3
.........value= 5F res = 5
.........value= 3XYZ res = 3
......to base is 10
.........value= 10 res = 8
.........value= 27 res = 23
.........value= 39 res = 3
.........value= 3 res = 3
.........value= 95 res = 5
.........value= 10 res = 8
.........value= 27 res = 23
.........value= 39 res = 3
.........value= 5F res = 5
.........value= 3XYZ res = 3
......to base is 16
.........value= 10 res = 8
.........value= 27 res = 17
.........value= 39 res = 3
.........value= 3 res = 3
.........value= 95 res = 5
.........value= 10 res = 8
.........value= 27 res = 17
.........value= 39 res = 3
.........value= 5F res = 5
.........value= 3XYZ res = 3
......to base is 36
.........value= 10 res = 8
.........value= 27 res = n
.........value= 39 res = 3
.........value= 3 res = 3
.........value= 95 res = 5
.........value= 10 res = 8
.........value= 27 res = n
.........value= 39 res = 3
.........value= 5F res = 5
.........value= 3XYZ res = 3

...from base is 10
......to base is 2
.........value= 10 res = 1010
.........value= 27 res = 11011
.........value= 39 res = 100111
.........value= 3 res = 11
.........value= 95 res = 1011111
.........value= 10 res = 1010
.........value= 27 res = 11011
.........value= 39 res = 100111
.........value= 5F res = 101
.........value= 3XYZ res = 11
......to base is 8
.........value= 10 res = 12
.........value= 27 res = 33
.........value= 39 res = 47
.........value= 3 res = 3
.........value= 95 res = 137
.........value= 10 res = 12
.........value= 27 res = 33
.........value= 39 res = 47
.........value= 5F res = 5
.........value= 3XYZ res = 3
......to base is 10
.........value= 10 res = 10
.........value= 27 res = 27
.........value= 39 res = 39
.........value= 3 res = 3
.........value= 95 res = 95
.........value= 10 res = 10
.........value= 27 res = 27
.........value= 39 res = 39
.........value= 5F res = 5
.........value= 3XYZ res = 3
......to base is 16
.........value= 10 res = a
.........value= 27 res = 1b
.........value= 39 res = 27
.........value= 3 res = 3
.........value= 95 res = 5f
.........value= 10 res = a
.........value= 27 res = 1b
.........value= 39 res = 27
.........value= 5F res = 5
.........value= 3XYZ res = 3
......to base is 36
.........value= 10 res = a
.........value= 27 res = r
.........value= 39 res = 13
.........value= 3 res = 3
.........value= 95 res = 2n
.........value= 10 res = a
.........value= 27 res = r
.........value= 39 res = 13
.........value= 5F res = 5
.........value= 3XYZ res = 3

...from base is 16
......to base is 2
.........value= 10 res = 10000
.........value= 27 res = 100111
.........value= 39 res = 111001
.........value= 3 res = 11
.........value= 95 res = 10010101
.........value= 10 res = 10000
.........value= 27 res = 100111
.........value= 39 res = 111001
.........value= 5F res = 1011111
.........value= 3XYZ res = 11
......to base is 8
.........value= 10 res = 20
.........value= 27 res = 47
.........value= 39 res = 71
.........value= 3 res = 3
.........value= 95 res = 225
.........value= 10 res = 20
.........value= 27 res = 47
.........value= 39 res = 71
.........value= 5F res = 137
.........value= 3XYZ res = 3
......to base is 10
.........value= 10 res = 16
.........value= 27 res = 39
.........value= 39 res = 57
.........value= 3 res = 3
.........value= 95 res = 149
.........value= 10 res = 16
.........value= 27 res = 39
.........value= 39 res = 57
.........value= 5F res = 95
.........value= 3XYZ res = 3
......to base is 16
.........value= 10 res = 10
.........value= 27 res = 27
.........value= 39 res = 39
.........value= 3 res = 3
.........value= 95 res = 95
.........value= 10 res = 10
.........value= 27 res = 27
.........value= 39 res = 39
.........value= 5F res = 5f
.........value= 3XYZ res = 3
......to base is 36
.........value= 10 res = g
.........value= 27 res = 13
.........value= 39 res = 1l
.........value= 3 res = 3
.........value= 95 res = 45
.........value= 10 res = g
.........value= 27 res = 13
.........value= 39 res = 1l
.........value= 5F res = 2n
.........value= 3XYZ res = 3

...from base is 36
......to base is 2
.........value= 10 res = 100100
.........value= 27 res = 1001111
.........value= 39 res = 1110101
.........value= 3 res = 11
.........value= 95 res = 101001001
.........value= 10 res = 100100
.........value= 27 res = 1001111
.........value= 39 res = 1110101
.........value= 5F res = 11000011
.........value= 3XYZ res = 101100111010111011
......to base is 8
.........value= 10 res = 44
.........value= 27 res = 117
.........value= 39 res = 165
.........value= 3 res = 3
.........value= 95 res = 511
.........value= 10 res = 44
.........value= 27 res = 117
.........value= 39 res = 165
.........value= 5F res = 303
.........value= 3XYZ res = 547273
......to base is 10
.........value= 10 res = 36
.........value= 27 res = 79
.........value= 39 res = 117
.........value= 3 res = 3
.........value= 95 res = 329
.........value= 10 res = 36
.........value= 27 res = 79
.........value= 39 res = 117
.........value= 5F res = 195
.........value= 3XYZ res = 183995
......to base is 16
.........value= 10 res = 24
.........value= 27 res = 4f
.........value= 39 res = 75
.........value= 3 res = 3
.........value= 95 res = 149
.........value= 10 res = 24
.........value= 27 res = 4f
.........value= 39 res = 75
.........value= 5F res = c3
.........value= 3XYZ res = 2cebb
......to base is 36
.........value= 10 res = 10
.........value= 27 res = 27
.........value= 39 res = 39
.........value= 3 res = 3
.........value= 95 res = 95
.........value= 10 res = 10
.........value= 27 res = 27
.........value= 39 res = 39
.........value= 5F res = 5f
.........value= 3XYZ res = 3xyz