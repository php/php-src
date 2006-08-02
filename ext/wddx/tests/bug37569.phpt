--TEST--
Bug #37569 (WDDX incorrectly encodes high-ascii characters)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php
for ($i = 65; $i < 256; $i++) {
	$v = chr($i);
	$ret = wddx_serialize_value($v);
	echo $ret . "\n";
	var_dump(ord($v), ord(wddx_deserialize($ret)), $v == wddx_deserialize($ret));
}
?>
--EXPECT--
<wddxPacket version='1.0'><header/><data><string>A</string></data></wddxPacket>
int(65)
int(65)
bool(true)
<wddxPacket version='1.0'><header/><data><string>B</string></data></wddxPacket>
int(66)
int(66)
bool(true)
<wddxPacket version='1.0'><header/><data><string>C</string></data></wddxPacket>
int(67)
int(67)
bool(true)
<wddxPacket version='1.0'><header/><data><string>D</string></data></wddxPacket>
int(68)
int(68)
bool(true)
<wddxPacket version='1.0'><header/><data><string>E</string></data></wddxPacket>
int(69)
int(69)
bool(true)
<wddxPacket version='1.0'><header/><data><string>F</string></data></wddxPacket>
int(70)
int(70)
bool(true)
<wddxPacket version='1.0'><header/><data><string>G</string></data></wddxPacket>
int(71)
int(71)
bool(true)
<wddxPacket version='1.0'><header/><data><string>H</string></data></wddxPacket>
int(72)
int(72)
bool(true)
<wddxPacket version='1.0'><header/><data><string>I</string></data></wddxPacket>
int(73)
int(73)
bool(true)
<wddxPacket version='1.0'><header/><data><string>J</string></data></wddxPacket>
int(74)
int(74)
bool(true)
<wddxPacket version='1.0'><header/><data><string>K</string></data></wddxPacket>
int(75)
int(75)
bool(true)
<wddxPacket version='1.0'><header/><data><string>L</string></data></wddxPacket>
int(76)
int(76)
bool(true)
<wddxPacket version='1.0'><header/><data><string>M</string></data></wddxPacket>
int(77)
int(77)
bool(true)
<wddxPacket version='1.0'><header/><data><string>N</string></data></wddxPacket>
int(78)
int(78)
bool(true)
<wddxPacket version='1.0'><header/><data><string>O</string></data></wddxPacket>
int(79)
int(79)
bool(true)
<wddxPacket version='1.0'><header/><data><string>P</string></data></wddxPacket>
int(80)
int(80)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Q</string></data></wddxPacket>
int(81)
int(81)
bool(true)
<wddxPacket version='1.0'><header/><data><string>R</string></data></wddxPacket>
int(82)
int(82)
bool(true)
<wddxPacket version='1.0'><header/><data><string>S</string></data></wddxPacket>
int(83)
int(83)
bool(true)
<wddxPacket version='1.0'><header/><data><string>T</string></data></wddxPacket>
int(84)
int(84)
bool(true)
<wddxPacket version='1.0'><header/><data><string>U</string></data></wddxPacket>
int(85)
int(85)
bool(true)
<wddxPacket version='1.0'><header/><data><string>V</string></data></wddxPacket>
int(86)
int(86)
bool(true)
<wddxPacket version='1.0'><header/><data><string>W</string></data></wddxPacket>
int(87)
int(87)
bool(true)
<wddxPacket version='1.0'><header/><data><string>X</string></data></wddxPacket>
int(88)
int(88)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Y</string></data></wddxPacket>
int(89)
int(89)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Z</string></data></wddxPacket>
int(90)
int(90)
bool(true)
<wddxPacket version='1.0'><header/><data><string>[</string></data></wddxPacket>
int(91)
int(91)
bool(true)
<wddxPacket version='1.0'><header/><data><string>\</string></data></wddxPacket>
int(92)
int(92)
bool(true)
<wddxPacket version='1.0'><header/><data><string>]</string></data></wddxPacket>
int(93)
int(93)
bool(true)
<wddxPacket version='1.0'><header/><data><string>^</string></data></wddxPacket>
int(94)
int(94)
bool(true)
<wddxPacket version='1.0'><header/><data><string>_</string></data></wddxPacket>
int(95)
int(95)
bool(true)
<wddxPacket version='1.0'><header/><data><string>`</string></data></wddxPacket>
int(96)
int(96)
bool(true)
<wddxPacket version='1.0'><header/><data><string>a</string></data></wddxPacket>
int(97)
int(97)
bool(true)
<wddxPacket version='1.0'><header/><data><string>b</string></data></wddxPacket>
int(98)
int(98)
bool(true)
<wddxPacket version='1.0'><header/><data><string>c</string></data></wddxPacket>
int(99)
int(99)
bool(true)
<wddxPacket version='1.0'><header/><data><string>d</string></data></wddxPacket>
int(100)
int(100)
bool(true)
<wddxPacket version='1.0'><header/><data><string>e</string></data></wddxPacket>
int(101)
int(101)
bool(true)
<wddxPacket version='1.0'><header/><data><string>f</string></data></wddxPacket>
int(102)
int(102)
bool(true)
<wddxPacket version='1.0'><header/><data><string>g</string></data></wddxPacket>
int(103)
int(103)
bool(true)
<wddxPacket version='1.0'><header/><data><string>h</string></data></wddxPacket>
int(104)
int(104)
bool(true)
<wddxPacket version='1.0'><header/><data><string>i</string></data></wddxPacket>
int(105)
int(105)
bool(true)
<wddxPacket version='1.0'><header/><data><string>j</string></data></wddxPacket>
int(106)
int(106)
bool(true)
<wddxPacket version='1.0'><header/><data><string>k</string></data></wddxPacket>
int(107)
int(107)
bool(true)
<wddxPacket version='1.0'><header/><data><string>l</string></data></wddxPacket>
int(108)
int(108)
bool(true)
<wddxPacket version='1.0'><header/><data><string>m</string></data></wddxPacket>
int(109)
int(109)
bool(true)
<wddxPacket version='1.0'><header/><data><string>n</string></data></wddxPacket>
int(110)
int(110)
bool(true)
<wddxPacket version='1.0'><header/><data><string>o</string></data></wddxPacket>
int(111)
int(111)
bool(true)
<wddxPacket version='1.0'><header/><data><string>p</string></data></wddxPacket>
int(112)
int(112)
bool(true)
<wddxPacket version='1.0'><header/><data><string>q</string></data></wddxPacket>
int(113)
int(113)
bool(true)
<wddxPacket version='1.0'><header/><data><string>r</string></data></wddxPacket>
int(114)
int(114)
bool(true)
<wddxPacket version='1.0'><header/><data><string>s</string></data></wddxPacket>
int(115)
int(115)
bool(true)
<wddxPacket version='1.0'><header/><data><string>t</string></data></wddxPacket>
int(116)
int(116)
bool(true)
<wddxPacket version='1.0'><header/><data><string>u</string></data></wddxPacket>
int(117)
int(117)
bool(true)
<wddxPacket version='1.0'><header/><data><string>v</string></data></wddxPacket>
int(118)
int(118)
bool(true)
<wddxPacket version='1.0'><header/><data><string>w</string></data></wddxPacket>
int(119)
int(119)
bool(true)
<wddxPacket version='1.0'><header/><data><string>x</string></data></wddxPacket>
int(120)
int(120)
bool(true)
<wddxPacket version='1.0'><header/><data><string>y</string></data></wddxPacket>
int(121)
int(121)
bool(true)
<wddxPacket version='1.0'><header/><data><string>z</string></data></wddxPacket>
int(122)
int(122)
bool(true)
<wddxPacket version='1.0'><header/><data><string>{</string></data></wddxPacket>
int(123)
int(123)
bool(true)
<wddxPacket version='1.0'><header/><data><string>|</string></data></wddxPacket>
int(124)
int(124)
bool(true)
<wddxPacket version='1.0'><header/><data><string>}</string></data></wddxPacket>
int(125)
int(125)
bool(true)
<wddxPacket version='1.0'><header/><data><string>~</string></data></wddxPacket>
int(126)
int(126)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(127)
int(127)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(128)
int(128)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(129)
int(129)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(130)
int(130)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(131)
int(131)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(132)
int(132)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(133)
int(133)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(134)
int(134)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(135)
int(135)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(136)
int(136)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(137)
int(137)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(138)
int(138)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(139)
int(139)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(140)
int(140)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(141)
int(141)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(142)
int(142)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(143)
int(143)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(144)
int(144)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(145)
int(145)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(146)
int(146)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(147)
int(147)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(148)
int(148)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(149)
int(149)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(150)
int(150)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(151)
int(151)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(152)
int(152)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(153)
int(153)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(154)
int(154)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(155)
int(155)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(156)
int(156)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(157)
int(157)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(158)
int(158)
bool(true)
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>
int(159)
int(159)
bool(true)
<wddxPacket version='1.0'><header/><data><string> </string></data></wddxPacket>
int(160)
int(160)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¡</string></data></wddxPacket>
int(161)
int(161)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¢</string></data></wddxPacket>
int(162)
int(162)
bool(true)
<wddxPacket version='1.0'><header/><data><string>£</string></data></wddxPacket>
int(163)
int(163)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¤</string></data></wddxPacket>
int(164)
int(164)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¥</string></data></wddxPacket>
int(165)
int(165)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¦</string></data></wddxPacket>
int(166)
int(166)
bool(true)
<wddxPacket version='1.0'><header/><data><string>§</string></data></wddxPacket>
int(167)
int(167)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¨</string></data></wddxPacket>
int(168)
int(168)
bool(true)
<wddxPacket version='1.0'><header/><data><string>©</string></data></wddxPacket>
int(169)
int(169)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ª</string></data></wddxPacket>
int(170)
int(170)
bool(true)
<wddxPacket version='1.0'><header/><data><string>«</string></data></wddxPacket>
int(171)
int(171)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¬</string></data></wddxPacket>
int(172)
int(172)
bool(true)
<wddxPacket version='1.0'><header/><data><string>­</string></data></wddxPacket>
int(173)
int(173)
bool(true)
<wddxPacket version='1.0'><header/><data><string>®</string></data></wddxPacket>
int(174)
int(174)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¯</string></data></wddxPacket>
int(175)
int(175)
bool(true)
<wddxPacket version='1.0'><header/><data><string>°</string></data></wddxPacket>
int(176)
int(176)
bool(true)
<wddxPacket version='1.0'><header/><data><string>±</string></data></wddxPacket>
int(177)
int(177)
bool(true)
<wddxPacket version='1.0'><header/><data><string>²</string></data></wddxPacket>
int(178)
int(178)
bool(true)
<wddxPacket version='1.0'><header/><data><string>³</string></data></wddxPacket>
int(179)
int(179)
bool(true)
<wddxPacket version='1.0'><header/><data><string>´</string></data></wddxPacket>
int(180)
int(180)
bool(true)
<wddxPacket version='1.0'><header/><data><string>µ</string></data></wddxPacket>
int(181)
int(181)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¶</string></data></wddxPacket>
int(182)
int(182)
bool(true)
<wddxPacket version='1.0'><header/><data><string>·</string></data></wddxPacket>
int(183)
int(183)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¸</string></data></wddxPacket>
int(184)
int(184)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¹</string></data></wddxPacket>
int(185)
int(185)
bool(true)
<wddxPacket version='1.0'><header/><data><string>º</string></data></wddxPacket>
int(186)
int(186)
bool(true)
<wddxPacket version='1.0'><header/><data><string>»</string></data></wddxPacket>
int(187)
int(187)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¼</string></data></wddxPacket>
int(188)
int(188)
bool(true)
<wddxPacket version='1.0'><header/><data><string>½</string></data></wddxPacket>
int(189)
int(189)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¾</string></data></wddxPacket>
int(190)
int(190)
bool(true)
<wddxPacket version='1.0'><header/><data><string>¿</string></data></wddxPacket>
int(191)
int(191)
bool(true)
<wddxPacket version='1.0'><header/><data><string>À</string></data></wddxPacket>
int(192)
int(192)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Á</string></data></wddxPacket>
int(193)
int(193)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Â</string></data></wddxPacket>
int(194)
int(194)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ã</string></data></wddxPacket>
int(195)
int(195)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ä</string></data></wddxPacket>
int(196)
int(196)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Å</string></data></wddxPacket>
int(197)
int(197)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Æ</string></data></wddxPacket>
int(198)
int(198)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ç</string></data></wddxPacket>
int(199)
int(199)
bool(true)
<wddxPacket version='1.0'><header/><data><string>È</string></data></wddxPacket>
int(200)
int(200)
bool(true)
<wddxPacket version='1.0'><header/><data><string>É</string></data></wddxPacket>
int(201)
int(201)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ê</string></data></wddxPacket>
int(202)
int(202)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ë</string></data></wddxPacket>
int(203)
int(203)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ì</string></data></wddxPacket>
int(204)
int(204)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Í</string></data></wddxPacket>
int(205)
int(205)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Î</string></data></wddxPacket>
int(206)
int(206)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ï</string></data></wddxPacket>
int(207)
int(207)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ð</string></data></wddxPacket>
int(208)
int(208)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ñ</string></data></wddxPacket>
int(209)
int(209)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ò</string></data></wddxPacket>
int(210)
int(210)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ó</string></data></wddxPacket>
int(211)
int(211)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ô</string></data></wddxPacket>
int(212)
int(212)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Õ</string></data></wddxPacket>
int(213)
int(213)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ö</string></data></wddxPacket>
int(214)
int(214)
bool(true)
<wddxPacket version='1.0'><header/><data><string>×</string></data></wddxPacket>
int(215)
int(215)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ø</string></data></wddxPacket>
int(216)
int(216)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ù</string></data></wddxPacket>
int(217)
int(217)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ú</string></data></wddxPacket>
int(218)
int(218)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Û</string></data></wddxPacket>
int(219)
int(219)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ü</string></data></wddxPacket>
int(220)
int(220)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Ý</string></data></wddxPacket>
int(221)
int(221)
bool(true)
<wddxPacket version='1.0'><header/><data><string>Þ</string></data></wddxPacket>
int(222)
int(222)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ß</string></data></wddxPacket>
int(223)
int(223)
bool(true)
<wddxPacket version='1.0'><header/><data><string>à</string></data></wddxPacket>
int(224)
int(224)
bool(true)
<wddxPacket version='1.0'><header/><data><string>á</string></data></wddxPacket>
int(225)
int(225)
bool(true)
<wddxPacket version='1.0'><header/><data><string>â</string></data></wddxPacket>
int(226)
int(226)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ã</string></data></wddxPacket>
int(227)
int(227)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ä</string></data></wddxPacket>
int(228)
int(228)
bool(true)
<wddxPacket version='1.0'><header/><data><string>å</string></data></wddxPacket>
int(229)
int(229)
bool(true)
<wddxPacket version='1.0'><header/><data><string>æ</string></data></wddxPacket>
int(230)
int(230)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ç</string></data></wddxPacket>
int(231)
int(231)
bool(true)
<wddxPacket version='1.0'><header/><data><string>è</string></data></wddxPacket>
int(232)
int(232)
bool(true)
<wddxPacket version='1.0'><header/><data><string>é</string></data></wddxPacket>
int(233)
int(233)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ê</string></data></wddxPacket>
int(234)
int(234)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ë</string></data></wddxPacket>
int(235)
int(235)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ì</string></data></wddxPacket>
int(236)
int(236)
bool(true)
<wddxPacket version='1.0'><header/><data><string>í</string></data></wddxPacket>
int(237)
int(237)
bool(true)
<wddxPacket version='1.0'><header/><data><string>î</string></data></wddxPacket>
int(238)
int(238)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ï</string></data></wddxPacket>
int(239)
int(239)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ð</string></data></wddxPacket>
int(240)
int(240)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ñ</string></data></wddxPacket>
int(241)
int(241)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ò</string></data></wddxPacket>
int(242)
int(242)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ó</string></data></wddxPacket>
int(243)
int(243)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ô</string></data></wddxPacket>
int(244)
int(244)
bool(true)
<wddxPacket version='1.0'><header/><data><string>õ</string></data></wddxPacket>
int(245)
int(245)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ö</string></data></wddxPacket>
int(246)
int(246)
bool(true)
<wddxPacket version='1.0'><header/><data><string>÷</string></data></wddxPacket>
int(247)
int(247)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ø</string></data></wddxPacket>
int(248)
int(248)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ù</string></data></wddxPacket>
int(249)
int(249)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ú</string></data></wddxPacket>
int(250)
int(250)
bool(true)
<wddxPacket version='1.0'><header/><data><string>û</string></data></wddxPacket>
int(251)
int(251)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ü</string></data></wddxPacket>
int(252)
int(252)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ý</string></data></wddxPacket>
int(253)
int(253)
bool(true)
<wddxPacket version='1.0'><header/><data><string>þ</string></data></wddxPacket>
int(254)
int(254)
bool(true)
<wddxPacket version='1.0'><header/><data><string>ÿ</string></data></wddxPacket>
int(255)
int(255)
bool(true)