--TEST--
Test umask() function: basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Linux');
}
?>
--FILE--
<?php
/* Prototype: int umask ( [int $mask] );
   Description: Changes the current umask
*/

echo "*** Testing umask() : basic functionality ***\n";
// checking umask() on all the modes
for($mask = 0000; $mask <= 0777; $mask++) {
  echo "-- Setting umask to $mask --\n";
  var_dump( umask($mask) );
  var_dump( umask() );
  echo "\n";
  if ($mask != umask()) {
    die('An error occurred while changing back the umask');
  }
}

echo "Done\n";
?>
--EXPECTF--
*** Testing umask() : basic functionality ***
-- Setting umask to 0 --
int(%d)
int(0)

-- Setting umask to 1 --
int(0)
int(1)

-- Setting umask to 2 --
int(1)
int(2)

-- Setting umask to 3 --
int(2)
int(3)

-- Setting umask to 4 --
int(3)
int(4)

-- Setting umask to 5 --
int(4)
int(5)

-- Setting umask to 6 --
int(5)
int(6)

-- Setting umask to 7 --
int(6)
int(7)

-- Setting umask to 8 --
int(7)
int(8)

-- Setting umask to 9 --
int(8)
int(9)

-- Setting umask to 10 --
int(9)
int(10)

-- Setting umask to 11 --
int(10)
int(11)

-- Setting umask to 12 --
int(11)
int(12)

-- Setting umask to 13 --
int(12)
int(13)

-- Setting umask to 14 --
int(13)
int(14)

-- Setting umask to 15 --
int(14)
int(15)

-- Setting umask to 16 --
int(15)
int(16)

-- Setting umask to 17 --
int(16)
int(17)

-- Setting umask to 18 --
int(17)
int(18)

-- Setting umask to 19 --
int(18)
int(19)

-- Setting umask to 20 --
int(19)
int(20)

-- Setting umask to 21 --
int(20)
int(21)

-- Setting umask to 22 --
int(21)
int(22)

-- Setting umask to 23 --
int(22)
int(23)

-- Setting umask to 24 --
int(23)
int(24)

-- Setting umask to 25 --
int(24)
int(25)

-- Setting umask to 26 --
int(25)
int(26)

-- Setting umask to 27 --
int(26)
int(27)

-- Setting umask to 28 --
int(27)
int(28)

-- Setting umask to 29 --
int(28)
int(29)

-- Setting umask to 30 --
int(29)
int(30)

-- Setting umask to 31 --
int(30)
int(31)

-- Setting umask to 32 --
int(31)
int(32)

-- Setting umask to 33 --
int(32)
int(33)

-- Setting umask to 34 --
int(33)
int(34)

-- Setting umask to 35 --
int(34)
int(35)

-- Setting umask to 36 --
int(35)
int(36)

-- Setting umask to 37 --
int(36)
int(37)

-- Setting umask to 38 --
int(37)
int(38)

-- Setting umask to 39 --
int(38)
int(39)

-- Setting umask to 40 --
int(39)
int(40)

-- Setting umask to 41 --
int(40)
int(41)

-- Setting umask to 42 --
int(41)
int(42)

-- Setting umask to 43 --
int(42)
int(43)

-- Setting umask to 44 --
int(43)
int(44)

-- Setting umask to 45 --
int(44)
int(45)

-- Setting umask to 46 --
int(45)
int(46)

-- Setting umask to 47 --
int(46)
int(47)

-- Setting umask to 48 --
int(47)
int(48)

-- Setting umask to 49 --
int(48)
int(49)

-- Setting umask to 50 --
int(49)
int(50)

-- Setting umask to 51 --
int(50)
int(51)

-- Setting umask to 52 --
int(51)
int(52)

-- Setting umask to 53 --
int(52)
int(53)

-- Setting umask to 54 --
int(53)
int(54)

-- Setting umask to 55 --
int(54)
int(55)

-- Setting umask to 56 --
int(55)
int(56)

-- Setting umask to 57 --
int(56)
int(57)

-- Setting umask to 58 --
int(57)
int(58)

-- Setting umask to 59 --
int(58)
int(59)

-- Setting umask to 60 --
int(59)
int(60)

-- Setting umask to 61 --
int(60)
int(61)

-- Setting umask to 62 --
int(61)
int(62)

-- Setting umask to 63 --
int(62)
int(63)

-- Setting umask to 64 --
int(63)
int(64)

-- Setting umask to 65 --
int(64)
int(65)

-- Setting umask to 66 --
int(65)
int(66)

-- Setting umask to 67 --
int(66)
int(67)

-- Setting umask to 68 --
int(67)
int(68)

-- Setting umask to 69 --
int(68)
int(69)

-- Setting umask to 70 --
int(69)
int(70)

-- Setting umask to 71 --
int(70)
int(71)

-- Setting umask to 72 --
int(71)
int(72)

-- Setting umask to 73 --
int(72)
int(73)

-- Setting umask to 74 --
int(73)
int(74)

-- Setting umask to 75 --
int(74)
int(75)

-- Setting umask to 76 --
int(75)
int(76)

-- Setting umask to 77 --
int(76)
int(77)

-- Setting umask to 78 --
int(77)
int(78)

-- Setting umask to 79 --
int(78)
int(79)

-- Setting umask to 80 --
int(79)
int(80)

-- Setting umask to 81 --
int(80)
int(81)

-- Setting umask to 82 --
int(81)
int(82)

-- Setting umask to 83 --
int(82)
int(83)

-- Setting umask to 84 --
int(83)
int(84)

-- Setting umask to 85 --
int(84)
int(85)

-- Setting umask to 86 --
int(85)
int(86)

-- Setting umask to 87 --
int(86)
int(87)

-- Setting umask to 88 --
int(87)
int(88)

-- Setting umask to 89 --
int(88)
int(89)

-- Setting umask to 90 --
int(89)
int(90)

-- Setting umask to 91 --
int(90)
int(91)

-- Setting umask to 92 --
int(91)
int(92)

-- Setting umask to 93 --
int(92)
int(93)

-- Setting umask to 94 --
int(93)
int(94)

-- Setting umask to 95 --
int(94)
int(95)

-- Setting umask to 96 --
int(95)
int(96)

-- Setting umask to 97 --
int(96)
int(97)

-- Setting umask to 98 --
int(97)
int(98)

-- Setting umask to 99 --
int(98)
int(99)

-- Setting umask to 100 --
int(99)
int(100)

-- Setting umask to 101 --
int(100)
int(101)

-- Setting umask to 102 --
int(101)
int(102)

-- Setting umask to 103 --
int(102)
int(103)

-- Setting umask to 104 --
int(103)
int(104)

-- Setting umask to 105 --
int(104)
int(105)

-- Setting umask to 106 --
int(105)
int(106)

-- Setting umask to 107 --
int(106)
int(107)

-- Setting umask to 108 --
int(107)
int(108)

-- Setting umask to 109 --
int(108)
int(109)

-- Setting umask to 110 --
int(109)
int(110)

-- Setting umask to 111 --
int(110)
int(111)

-- Setting umask to 112 --
int(111)
int(112)

-- Setting umask to 113 --
int(112)
int(113)

-- Setting umask to 114 --
int(113)
int(114)

-- Setting umask to 115 --
int(114)
int(115)

-- Setting umask to 116 --
int(115)
int(116)

-- Setting umask to 117 --
int(116)
int(117)

-- Setting umask to 118 --
int(117)
int(118)

-- Setting umask to 119 --
int(118)
int(119)

-- Setting umask to 120 --
int(119)
int(120)

-- Setting umask to 121 --
int(120)
int(121)

-- Setting umask to 122 --
int(121)
int(122)

-- Setting umask to 123 --
int(122)
int(123)

-- Setting umask to 124 --
int(123)
int(124)

-- Setting umask to 125 --
int(124)
int(125)

-- Setting umask to 126 --
int(125)
int(126)

-- Setting umask to 127 --
int(126)
int(127)

-- Setting umask to 128 --
int(127)
int(128)

-- Setting umask to 129 --
int(128)
int(129)

-- Setting umask to 130 --
int(129)
int(130)

-- Setting umask to 131 --
int(130)
int(131)

-- Setting umask to 132 --
int(131)
int(132)

-- Setting umask to 133 --
int(132)
int(133)

-- Setting umask to 134 --
int(133)
int(134)

-- Setting umask to 135 --
int(134)
int(135)

-- Setting umask to 136 --
int(135)
int(136)

-- Setting umask to 137 --
int(136)
int(137)

-- Setting umask to 138 --
int(137)
int(138)

-- Setting umask to 139 --
int(138)
int(139)

-- Setting umask to 140 --
int(139)
int(140)

-- Setting umask to 141 --
int(140)
int(141)

-- Setting umask to 142 --
int(141)
int(142)

-- Setting umask to 143 --
int(142)
int(143)

-- Setting umask to 144 --
int(143)
int(144)

-- Setting umask to 145 --
int(144)
int(145)

-- Setting umask to 146 --
int(145)
int(146)

-- Setting umask to 147 --
int(146)
int(147)

-- Setting umask to 148 --
int(147)
int(148)

-- Setting umask to 149 --
int(148)
int(149)

-- Setting umask to 150 --
int(149)
int(150)

-- Setting umask to 151 --
int(150)
int(151)

-- Setting umask to 152 --
int(151)
int(152)

-- Setting umask to 153 --
int(152)
int(153)

-- Setting umask to 154 --
int(153)
int(154)

-- Setting umask to 155 --
int(154)
int(155)

-- Setting umask to 156 --
int(155)
int(156)

-- Setting umask to 157 --
int(156)
int(157)

-- Setting umask to 158 --
int(157)
int(158)

-- Setting umask to 159 --
int(158)
int(159)

-- Setting umask to 160 --
int(159)
int(160)

-- Setting umask to 161 --
int(160)
int(161)

-- Setting umask to 162 --
int(161)
int(162)

-- Setting umask to 163 --
int(162)
int(163)

-- Setting umask to 164 --
int(163)
int(164)

-- Setting umask to 165 --
int(164)
int(165)

-- Setting umask to 166 --
int(165)
int(166)

-- Setting umask to 167 --
int(166)
int(167)

-- Setting umask to 168 --
int(167)
int(168)

-- Setting umask to 169 --
int(168)
int(169)

-- Setting umask to 170 --
int(169)
int(170)

-- Setting umask to 171 --
int(170)
int(171)

-- Setting umask to 172 --
int(171)
int(172)

-- Setting umask to 173 --
int(172)
int(173)

-- Setting umask to 174 --
int(173)
int(174)

-- Setting umask to 175 --
int(174)
int(175)

-- Setting umask to 176 --
int(175)
int(176)

-- Setting umask to 177 --
int(176)
int(177)

-- Setting umask to 178 --
int(177)
int(178)

-- Setting umask to 179 --
int(178)
int(179)

-- Setting umask to 180 --
int(179)
int(180)

-- Setting umask to 181 --
int(180)
int(181)

-- Setting umask to 182 --
int(181)
int(182)

-- Setting umask to 183 --
int(182)
int(183)

-- Setting umask to 184 --
int(183)
int(184)

-- Setting umask to 185 --
int(184)
int(185)

-- Setting umask to 186 --
int(185)
int(186)

-- Setting umask to 187 --
int(186)
int(187)

-- Setting umask to 188 --
int(187)
int(188)

-- Setting umask to 189 --
int(188)
int(189)

-- Setting umask to 190 --
int(189)
int(190)

-- Setting umask to 191 --
int(190)
int(191)

-- Setting umask to 192 --
int(191)
int(192)

-- Setting umask to 193 --
int(192)
int(193)

-- Setting umask to 194 --
int(193)
int(194)

-- Setting umask to 195 --
int(194)
int(195)

-- Setting umask to 196 --
int(195)
int(196)

-- Setting umask to 197 --
int(196)
int(197)

-- Setting umask to 198 --
int(197)
int(198)

-- Setting umask to 199 --
int(198)
int(199)

-- Setting umask to 200 --
int(199)
int(200)

-- Setting umask to 201 --
int(200)
int(201)

-- Setting umask to 202 --
int(201)
int(202)

-- Setting umask to 203 --
int(202)
int(203)

-- Setting umask to 204 --
int(203)
int(204)

-- Setting umask to 205 --
int(204)
int(205)

-- Setting umask to 206 --
int(205)
int(206)

-- Setting umask to 207 --
int(206)
int(207)

-- Setting umask to 208 --
int(207)
int(208)

-- Setting umask to 209 --
int(208)
int(209)

-- Setting umask to 210 --
int(209)
int(210)

-- Setting umask to 211 --
int(210)
int(211)

-- Setting umask to 212 --
int(211)
int(212)

-- Setting umask to 213 --
int(212)
int(213)

-- Setting umask to 214 --
int(213)
int(214)

-- Setting umask to 215 --
int(214)
int(215)

-- Setting umask to 216 --
int(215)
int(216)

-- Setting umask to 217 --
int(216)
int(217)

-- Setting umask to 218 --
int(217)
int(218)

-- Setting umask to 219 --
int(218)
int(219)

-- Setting umask to 220 --
int(219)
int(220)

-- Setting umask to 221 --
int(220)
int(221)

-- Setting umask to 222 --
int(221)
int(222)

-- Setting umask to 223 --
int(222)
int(223)

-- Setting umask to 224 --
int(223)
int(224)

-- Setting umask to 225 --
int(224)
int(225)

-- Setting umask to 226 --
int(225)
int(226)

-- Setting umask to 227 --
int(226)
int(227)

-- Setting umask to 228 --
int(227)
int(228)

-- Setting umask to 229 --
int(228)
int(229)

-- Setting umask to 230 --
int(229)
int(230)

-- Setting umask to 231 --
int(230)
int(231)

-- Setting umask to 232 --
int(231)
int(232)

-- Setting umask to 233 --
int(232)
int(233)

-- Setting umask to 234 --
int(233)
int(234)

-- Setting umask to 235 --
int(234)
int(235)

-- Setting umask to 236 --
int(235)
int(236)

-- Setting umask to 237 --
int(236)
int(237)

-- Setting umask to 238 --
int(237)
int(238)

-- Setting umask to 239 --
int(238)
int(239)

-- Setting umask to 240 --
int(239)
int(240)

-- Setting umask to 241 --
int(240)
int(241)

-- Setting umask to 242 --
int(241)
int(242)

-- Setting umask to 243 --
int(242)
int(243)

-- Setting umask to 244 --
int(243)
int(244)

-- Setting umask to 245 --
int(244)
int(245)

-- Setting umask to 246 --
int(245)
int(246)

-- Setting umask to 247 --
int(246)
int(247)

-- Setting umask to 248 --
int(247)
int(248)

-- Setting umask to 249 --
int(248)
int(249)

-- Setting umask to 250 --
int(249)
int(250)

-- Setting umask to 251 --
int(250)
int(251)

-- Setting umask to 252 --
int(251)
int(252)

-- Setting umask to 253 --
int(252)
int(253)

-- Setting umask to 254 --
int(253)
int(254)

-- Setting umask to 255 --
int(254)
int(255)

-- Setting umask to 256 --
int(255)
int(256)

-- Setting umask to 257 --
int(256)
int(257)

-- Setting umask to 258 --
int(257)
int(258)

-- Setting umask to 259 --
int(258)
int(259)

-- Setting umask to 260 --
int(259)
int(260)

-- Setting umask to 261 --
int(260)
int(261)

-- Setting umask to 262 --
int(261)
int(262)

-- Setting umask to 263 --
int(262)
int(263)

-- Setting umask to 264 --
int(263)
int(264)

-- Setting umask to 265 --
int(264)
int(265)

-- Setting umask to 266 --
int(265)
int(266)

-- Setting umask to 267 --
int(266)
int(267)

-- Setting umask to 268 --
int(267)
int(268)

-- Setting umask to 269 --
int(268)
int(269)

-- Setting umask to 270 --
int(269)
int(270)

-- Setting umask to 271 --
int(270)
int(271)

-- Setting umask to 272 --
int(271)
int(272)

-- Setting umask to 273 --
int(272)
int(273)

-- Setting umask to 274 --
int(273)
int(274)

-- Setting umask to 275 --
int(274)
int(275)

-- Setting umask to 276 --
int(275)
int(276)

-- Setting umask to 277 --
int(276)
int(277)

-- Setting umask to 278 --
int(277)
int(278)

-- Setting umask to 279 --
int(278)
int(279)

-- Setting umask to 280 --
int(279)
int(280)

-- Setting umask to 281 --
int(280)
int(281)

-- Setting umask to 282 --
int(281)
int(282)

-- Setting umask to 283 --
int(282)
int(283)

-- Setting umask to 284 --
int(283)
int(284)

-- Setting umask to 285 --
int(284)
int(285)

-- Setting umask to 286 --
int(285)
int(286)

-- Setting umask to 287 --
int(286)
int(287)

-- Setting umask to 288 --
int(287)
int(288)

-- Setting umask to 289 --
int(288)
int(289)

-- Setting umask to 290 --
int(289)
int(290)

-- Setting umask to 291 --
int(290)
int(291)

-- Setting umask to 292 --
int(291)
int(292)

-- Setting umask to 293 --
int(292)
int(293)

-- Setting umask to 294 --
int(293)
int(294)

-- Setting umask to 295 --
int(294)
int(295)

-- Setting umask to 296 --
int(295)
int(296)

-- Setting umask to 297 --
int(296)
int(297)

-- Setting umask to 298 --
int(297)
int(298)

-- Setting umask to 299 --
int(298)
int(299)

-- Setting umask to 300 --
int(299)
int(300)

-- Setting umask to 301 --
int(300)
int(301)

-- Setting umask to 302 --
int(301)
int(302)

-- Setting umask to 303 --
int(302)
int(303)

-- Setting umask to 304 --
int(303)
int(304)

-- Setting umask to 305 --
int(304)
int(305)

-- Setting umask to 306 --
int(305)
int(306)

-- Setting umask to 307 --
int(306)
int(307)

-- Setting umask to 308 --
int(307)
int(308)

-- Setting umask to 309 --
int(308)
int(309)

-- Setting umask to 310 --
int(309)
int(310)

-- Setting umask to 311 --
int(310)
int(311)

-- Setting umask to 312 --
int(311)
int(312)

-- Setting umask to 313 --
int(312)
int(313)

-- Setting umask to 314 --
int(313)
int(314)

-- Setting umask to 315 --
int(314)
int(315)

-- Setting umask to 316 --
int(315)
int(316)

-- Setting umask to 317 --
int(316)
int(317)

-- Setting umask to 318 --
int(317)
int(318)

-- Setting umask to 319 --
int(318)
int(319)

-- Setting umask to 320 --
int(319)
int(320)

-- Setting umask to 321 --
int(320)
int(321)

-- Setting umask to 322 --
int(321)
int(322)

-- Setting umask to 323 --
int(322)
int(323)

-- Setting umask to 324 --
int(323)
int(324)

-- Setting umask to 325 --
int(324)
int(325)

-- Setting umask to 326 --
int(325)
int(326)

-- Setting umask to 327 --
int(326)
int(327)

-- Setting umask to 328 --
int(327)
int(328)

-- Setting umask to 329 --
int(328)
int(329)

-- Setting umask to 330 --
int(329)
int(330)

-- Setting umask to 331 --
int(330)
int(331)

-- Setting umask to 332 --
int(331)
int(332)

-- Setting umask to 333 --
int(332)
int(333)

-- Setting umask to 334 --
int(333)
int(334)

-- Setting umask to 335 --
int(334)
int(335)

-- Setting umask to 336 --
int(335)
int(336)

-- Setting umask to 337 --
int(336)
int(337)

-- Setting umask to 338 --
int(337)
int(338)

-- Setting umask to 339 --
int(338)
int(339)

-- Setting umask to 340 --
int(339)
int(340)

-- Setting umask to 341 --
int(340)
int(341)

-- Setting umask to 342 --
int(341)
int(342)

-- Setting umask to 343 --
int(342)
int(343)

-- Setting umask to 344 --
int(343)
int(344)

-- Setting umask to 345 --
int(344)
int(345)

-- Setting umask to 346 --
int(345)
int(346)

-- Setting umask to 347 --
int(346)
int(347)

-- Setting umask to 348 --
int(347)
int(348)

-- Setting umask to 349 --
int(348)
int(349)

-- Setting umask to 350 --
int(349)
int(350)

-- Setting umask to 351 --
int(350)
int(351)

-- Setting umask to 352 --
int(351)
int(352)

-- Setting umask to 353 --
int(352)
int(353)

-- Setting umask to 354 --
int(353)
int(354)

-- Setting umask to 355 --
int(354)
int(355)

-- Setting umask to 356 --
int(355)
int(356)

-- Setting umask to 357 --
int(356)
int(357)

-- Setting umask to 358 --
int(357)
int(358)

-- Setting umask to 359 --
int(358)
int(359)

-- Setting umask to 360 --
int(359)
int(360)

-- Setting umask to 361 --
int(360)
int(361)

-- Setting umask to 362 --
int(361)
int(362)

-- Setting umask to 363 --
int(362)
int(363)

-- Setting umask to 364 --
int(363)
int(364)

-- Setting umask to 365 --
int(364)
int(365)

-- Setting umask to 366 --
int(365)
int(366)

-- Setting umask to 367 --
int(366)
int(367)

-- Setting umask to 368 --
int(367)
int(368)

-- Setting umask to 369 --
int(368)
int(369)

-- Setting umask to 370 --
int(369)
int(370)

-- Setting umask to 371 --
int(370)
int(371)

-- Setting umask to 372 --
int(371)
int(372)

-- Setting umask to 373 --
int(372)
int(373)

-- Setting umask to 374 --
int(373)
int(374)

-- Setting umask to 375 --
int(374)
int(375)

-- Setting umask to 376 --
int(375)
int(376)

-- Setting umask to 377 --
int(376)
int(377)

-- Setting umask to 378 --
int(377)
int(378)

-- Setting umask to 379 --
int(378)
int(379)

-- Setting umask to 380 --
int(379)
int(380)

-- Setting umask to 381 --
int(380)
int(381)

-- Setting umask to 382 --
int(381)
int(382)

-- Setting umask to 383 --
int(382)
int(383)

-- Setting umask to 384 --
int(383)
int(384)

-- Setting umask to 385 --
int(384)
int(385)

-- Setting umask to 386 --
int(385)
int(386)

-- Setting umask to 387 --
int(386)
int(387)

-- Setting umask to 388 --
int(387)
int(388)

-- Setting umask to 389 --
int(388)
int(389)

-- Setting umask to 390 --
int(389)
int(390)

-- Setting umask to 391 --
int(390)
int(391)

-- Setting umask to 392 --
int(391)
int(392)

-- Setting umask to 393 --
int(392)
int(393)

-- Setting umask to 394 --
int(393)
int(394)

-- Setting umask to 395 --
int(394)
int(395)

-- Setting umask to 396 --
int(395)
int(396)

-- Setting umask to 397 --
int(396)
int(397)

-- Setting umask to 398 --
int(397)
int(398)

-- Setting umask to 399 --
int(398)
int(399)

-- Setting umask to 400 --
int(399)
int(400)

-- Setting umask to 401 --
int(400)
int(401)

-- Setting umask to 402 --
int(401)
int(402)

-- Setting umask to 403 --
int(402)
int(403)

-- Setting umask to 404 --
int(403)
int(404)

-- Setting umask to 405 --
int(404)
int(405)

-- Setting umask to 406 --
int(405)
int(406)

-- Setting umask to 407 --
int(406)
int(407)

-- Setting umask to 408 --
int(407)
int(408)

-- Setting umask to 409 --
int(408)
int(409)

-- Setting umask to 410 --
int(409)
int(410)

-- Setting umask to 411 --
int(410)
int(411)

-- Setting umask to 412 --
int(411)
int(412)

-- Setting umask to 413 --
int(412)
int(413)

-- Setting umask to 414 --
int(413)
int(414)

-- Setting umask to 415 --
int(414)
int(415)

-- Setting umask to 416 --
int(415)
int(416)

-- Setting umask to 417 --
int(416)
int(417)

-- Setting umask to 418 --
int(417)
int(418)

-- Setting umask to 419 --
int(418)
int(419)

-- Setting umask to 420 --
int(419)
int(420)

-- Setting umask to 421 --
int(420)
int(421)

-- Setting umask to 422 --
int(421)
int(422)

-- Setting umask to 423 --
int(422)
int(423)

-- Setting umask to 424 --
int(423)
int(424)

-- Setting umask to 425 --
int(424)
int(425)

-- Setting umask to 426 --
int(425)
int(426)

-- Setting umask to 427 --
int(426)
int(427)

-- Setting umask to 428 --
int(427)
int(428)

-- Setting umask to 429 --
int(428)
int(429)

-- Setting umask to 430 --
int(429)
int(430)

-- Setting umask to 431 --
int(430)
int(431)

-- Setting umask to 432 --
int(431)
int(432)

-- Setting umask to 433 --
int(432)
int(433)

-- Setting umask to 434 --
int(433)
int(434)

-- Setting umask to 435 --
int(434)
int(435)

-- Setting umask to 436 --
int(435)
int(436)

-- Setting umask to 437 --
int(436)
int(437)

-- Setting umask to 438 --
int(437)
int(438)

-- Setting umask to 439 --
int(438)
int(439)

-- Setting umask to 440 --
int(439)
int(440)

-- Setting umask to 441 --
int(440)
int(441)

-- Setting umask to 442 --
int(441)
int(442)

-- Setting umask to 443 --
int(442)
int(443)

-- Setting umask to 444 --
int(443)
int(444)

-- Setting umask to 445 --
int(444)
int(445)

-- Setting umask to 446 --
int(445)
int(446)

-- Setting umask to 447 --
int(446)
int(447)

-- Setting umask to 448 --
int(447)
int(448)

-- Setting umask to 449 --
int(448)
int(449)

-- Setting umask to 450 --
int(449)
int(450)

-- Setting umask to 451 --
int(450)
int(451)

-- Setting umask to 452 --
int(451)
int(452)

-- Setting umask to 453 --
int(452)
int(453)

-- Setting umask to 454 --
int(453)
int(454)

-- Setting umask to 455 --
int(454)
int(455)

-- Setting umask to 456 --
int(455)
int(456)

-- Setting umask to 457 --
int(456)
int(457)

-- Setting umask to 458 --
int(457)
int(458)

-- Setting umask to 459 --
int(458)
int(459)

-- Setting umask to 460 --
int(459)
int(460)

-- Setting umask to 461 --
int(460)
int(461)

-- Setting umask to 462 --
int(461)
int(462)

-- Setting umask to 463 --
int(462)
int(463)

-- Setting umask to 464 --
int(463)
int(464)

-- Setting umask to 465 --
int(464)
int(465)

-- Setting umask to 466 --
int(465)
int(466)

-- Setting umask to 467 --
int(466)
int(467)

-- Setting umask to 468 --
int(467)
int(468)

-- Setting umask to 469 --
int(468)
int(469)

-- Setting umask to 470 --
int(469)
int(470)

-- Setting umask to 471 --
int(470)
int(471)

-- Setting umask to 472 --
int(471)
int(472)

-- Setting umask to 473 --
int(472)
int(473)

-- Setting umask to 474 --
int(473)
int(474)

-- Setting umask to 475 --
int(474)
int(475)

-- Setting umask to 476 --
int(475)
int(476)

-- Setting umask to 477 --
int(476)
int(477)

-- Setting umask to 478 --
int(477)
int(478)

-- Setting umask to 479 --
int(478)
int(479)

-- Setting umask to 480 --
int(479)
int(480)

-- Setting umask to 481 --
int(480)
int(481)

-- Setting umask to 482 --
int(481)
int(482)

-- Setting umask to 483 --
int(482)
int(483)

-- Setting umask to 484 --
int(483)
int(484)

-- Setting umask to 485 --
int(484)
int(485)

-- Setting umask to 486 --
int(485)
int(486)

-- Setting umask to 487 --
int(486)
int(487)

-- Setting umask to 488 --
int(487)
int(488)

-- Setting umask to 489 --
int(488)
int(489)

-- Setting umask to 490 --
int(489)
int(490)

-- Setting umask to 491 --
int(490)
int(491)

-- Setting umask to 492 --
int(491)
int(492)

-- Setting umask to 493 --
int(492)
int(493)

-- Setting umask to 494 --
int(493)
int(494)

-- Setting umask to 495 --
int(494)
int(495)

-- Setting umask to 496 --
int(495)
int(496)

-- Setting umask to 497 --
int(496)
int(497)

-- Setting umask to 498 --
int(497)
int(498)

-- Setting umask to 499 --
int(498)
int(499)

-- Setting umask to 500 --
int(499)
int(500)

-- Setting umask to 501 --
int(500)
int(501)

-- Setting umask to 502 --
int(501)
int(502)

-- Setting umask to 503 --
int(502)
int(503)

-- Setting umask to 504 --
int(503)
int(504)

-- Setting umask to 505 --
int(504)
int(505)

-- Setting umask to 506 --
int(505)
int(506)

-- Setting umask to 507 --
int(506)
int(507)

-- Setting umask to 508 --
int(507)
int(508)

-- Setting umask to 509 --
int(508)
int(509)

-- Setting umask to 510 --
int(509)
int(510)

-- Setting umask to 511 --
int(510)
int(511)

Done
