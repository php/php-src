--TEST--
version_compare test
--FILE--
<?php

$special_forms = array("-dev", "a1", "b1", "RC1", "", "pl1");
test("1", "2");
test("10", "2");
test("1.0", "1.1");
test("1.2", "1.0.1");
foreach ($special_forms as $f1) {
    foreach ($special_forms as $f2) {
	test("1.0$f1", "1.0$f2");
    }
}


function test($v1, $v2) {
    $compare = version_compare($v1, $v2);
    switch ($compare) {
	case -1:
	    print "$v1 < $v2\n";
	    break;
	case 1:
	    print "$v1 > $v2\n";
	    break;
	case 0:
	default:
	    print "$v1 = $v2\n";
	    break;
    }
}

?>
--EXPECT--
1 < 2
10 > 2
1.0 < 1.1
1.2 > 1.0.1
1.0-dev = 1.0-dev
1.0-dev < 1.0a1
1.0-dev < 1.0b1
1.0-dev < 1.0RC1
1.0-dev < 1.0
1.0-dev < 1.0pl1
1.0a1 > 1.0-dev
1.0a1 = 1.0a1
1.0a1 < 1.0b1
1.0a1 < 1.0RC1
1.0a1 < 1.0
1.0a1 < 1.0pl1
1.0b1 > 1.0-dev
1.0b1 > 1.0a1
1.0b1 = 1.0b1
1.0b1 < 1.0RC1
1.0b1 < 1.0
1.0b1 < 1.0pl1
1.0RC1 > 1.0-dev
1.0RC1 > 1.0a1
1.0RC1 > 1.0b1
1.0RC1 = 1.0RC1
1.0RC1 < 1.0
1.0RC1 < 1.0pl1
1.0 > 1.0-dev
1.0 > 1.0a1
1.0 > 1.0b1
1.0 > 1.0RC1
1.0 = 1.0
1.0 < 1.0pl1
1.0pl1 > 1.0-dev
1.0pl1 > 1.0a1
1.0pl1 > 1.0b1
1.0pl1 > 1.0RC1
1.0pl1 > 1.0
1.0pl1 = 1.0pl1
