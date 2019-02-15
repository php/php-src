--TEST--
yield from on multiple trees needing merge
--FILE--
<?php

function from($levels) {
	foreach (range(0, 2 << $levels) as $v) {
		yield $v;
	}
}

function gen($gen, $level) {
	if ($level % 2) {
		yield $gen->current();
	}
	yield from $gen;
}

foreach (range(0, 6) as $levels) {
	print "$levels level".($levels == 1 ? "" : "s")."\n\n";

	$all = array();
	$all[] = $gens[0][0] = from($levels);

	for ($level = 1; $level < $levels; $level++) {
		for ($i = 0; $i < (1 << $level); $i++) {
			$all[] = $gens[$level][$i] = gen($gens[$level-1][$i >> 1], $level);
		}
	}

	while (1) {
		foreach ($all as $gen) {
			var_dump($gen->current());
			$gen->next();
			if (!$gen->valid()) {
				break 2;
			}
		}
	}

	print "\n\n";
}
?>
--EXPECT--
0 levels

int(0)
int(1)
int(2)


1 level

int(0)
int(1)
int(2)
int(3)
int(4)


2 levels

int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)


3 levels

int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
int(11)
int(12)
int(13)
int(14)
int(15)
int(16)


4 levels

int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
int(11)
int(12)
int(13)
int(14)
int(15)
int(16)
int(17)
int(18)
int(19)
int(20)
int(21)
int(22)
int(23)
int(24)
int(25)
int(26)
int(27)
int(28)
int(29)
int(30)
int(31)
int(32)


5 levels

int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
int(11)
int(12)
int(13)
int(14)
int(15)
int(16)
int(17)
int(18)
int(19)
int(20)
int(21)
int(22)
int(23)
int(24)
int(25)
int(26)
int(27)
int(28)
int(29)
int(30)
int(31)
int(32)
int(33)
int(34)
int(35)
int(36)
int(37)
int(38)
int(39)
int(40)
int(41)
int(42)
int(43)
int(44)
int(45)
int(46)
int(47)
int(48)
int(49)
int(50)
int(51)
int(52)
int(53)
int(54)
int(55)
int(56)
int(57)
int(58)
int(59)
int(60)
int(61)
int(62)
int(63)
int(64)


6 levels

int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
int(11)
int(12)
int(13)
int(14)
int(15)
int(16)
int(17)
int(18)
int(19)
int(20)
int(21)
int(22)
int(23)
int(24)
int(25)
int(26)
int(27)
int(28)
int(29)
int(30)
int(31)
int(32)
int(33)
int(34)
int(35)
int(36)
int(37)
int(38)
int(39)
int(40)
int(41)
int(42)
int(43)
int(44)
int(45)
int(46)
int(47)
int(48)
int(49)
int(50)
int(51)
int(52)
int(53)
int(54)
int(55)
int(56)
int(57)
int(58)
int(59)
int(60)
int(61)
int(62)
int(63)
int(64)
int(65)
int(66)
int(67)
int(68)
int(69)
int(70)
int(71)
int(72)
int(73)
int(74)
int(75)
int(76)
int(77)
int(78)
int(79)
int(80)
int(81)
int(82)
int(83)
int(84)
int(85)
int(86)
int(87)
int(88)
int(89)
int(90)
int(91)
int(92)
int(93)
int(94)
int(95)
int(96)
int(97)
int(98)
int(99)
int(100)
int(101)
int(102)
int(103)
int(104)
int(105)
int(106)
int(107)
int(108)
int(109)
int(110)
int(111)
int(112)
int(113)
int(114)
int(115)
int(116)
int(117)
int(118)
int(119)
int(120)
int(121)
int(122)
int(123)
int(124)
int(125)
int(126)
int(127)
int(128)
