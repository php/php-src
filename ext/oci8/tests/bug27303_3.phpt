--TEST--
Bug #27303 (OCIBindByName binds numeric PHP values as characters)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
	
$create_st = array();
$create_st[] = "drop sequence myseq";
$create_st[] = "drop table mytab";
$create_st[] = "create sequence myseq";
$create_st[] = "create table mytab (mydata varchar2(20), seqcol number)";

foreach ($create_st as $statement) {
	$stmt = oci_parse($c, $statement);
	@oci_execute($stmt);
}

define('MYLIMIT', 200);
define('INITMYBV', 11);

$stmt = "insert into mytab (mydata, seqcol) values ('Some data', myseq.nextval) returning seqcol into :mybv";

$stid = OCIParse($c, $stmt);
if (!$stid) { echo "Parse error"; die; }

$mybv = INITMYBV;
$r = OCIBindByName($stid, ':MYBV', $mybv, 5, SQLT_INT);
if (!$r) { echo "Bind error"; die; }

for ($i = 1; $i < MYLIMIT; $i++) {
	$r = OCIExecute($stid, OCI_DEFAULT);
	if (!$r) { echo "Execute error"; die; }
	var_dump($mybv);
}

OCICommit($c);

$drop_st = array();
$drop_st[] = "drop sequence myseq";
$drop_st[] = "drop table mytab";

foreach ($create_st as $statement) {
	$stmt = oci_parse($c, $statement);
	oci_execute($stmt);
}

echo "Done\n";
?>
--EXPECTF--	
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
int(129)
int(130)
int(131)
int(132)
int(133)
int(134)
int(135)
int(136)
int(137)
int(138)
int(139)
int(140)
int(141)
int(142)
int(143)
int(144)
int(145)
int(146)
int(147)
int(148)
int(149)
int(150)
int(151)
int(152)
int(153)
int(154)
int(155)
int(156)
int(157)
int(158)
int(159)
int(160)
int(161)
int(162)
int(163)
int(164)
int(165)
int(166)
int(167)
int(168)
int(169)
int(170)
int(171)
int(172)
int(173)
int(174)
int(175)
int(176)
int(177)
int(178)
int(179)
int(180)
int(181)
int(182)
int(183)
int(184)
int(185)
int(186)
int(187)
int(188)
int(189)
int(190)
int(191)
int(192)
int(193)
int(194)
int(195)
int(196)
int(197)
int(198)
int(199)
Done
