--TEST--
Random: Randomizer: Nul engines are correctly handled
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class NulEngine implements Engine
{
    public function generate(): string
    {
        return str_repeat("\x00", PHP_INT_SIZE);
    }
}

function randomizer(): Randomizer
{
    return new Randomizer(new NulEngine());
}

try {
    var_dump(randomizer()->getInt(0, 1234));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->nextInt());
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(bin2hex(randomizer()->getBytes(1)));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->shuffleArray(range(1, 123)));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->pickArrayKeys(range(1, 123), 1));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->pickArrayKeys(range(1, 123), 10));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->shuffleBytes('foobar'));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
int(0)
int(0)
string(2) "00"
array(123) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(6)
  [5]=>
  int(7)
  [6]=>
  int(8)
  [7]=>
  int(9)
  [8]=>
  int(10)
  [9]=>
  int(11)
  [10]=>
  int(12)
  [11]=>
  int(13)
  [12]=>
  int(14)
  [13]=>
  int(15)
  [14]=>
  int(16)
  [15]=>
  int(17)
  [16]=>
  int(18)
  [17]=>
  int(19)
  [18]=>
  int(20)
  [19]=>
  int(21)
  [20]=>
  int(22)
  [21]=>
  int(23)
  [22]=>
  int(24)
  [23]=>
  int(25)
  [24]=>
  int(26)
  [25]=>
  int(27)
  [26]=>
  int(28)
  [27]=>
  int(29)
  [28]=>
  int(30)
  [29]=>
  int(31)
  [30]=>
  int(32)
  [31]=>
  int(33)
  [32]=>
  int(34)
  [33]=>
  int(35)
  [34]=>
  int(36)
  [35]=>
  int(37)
  [36]=>
  int(38)
  [37]=>
  int(39)
  [38]=>
  int(40)
  [39]=>
  int(41)
  [40]=>
  int(42)
  [41]=>
  int(43)
  [42]=>
  int(44)
  [43]=>
  int(45)
  [44]=>
  int(46)
  [45]=>
  int(47)
  [46]=>
  int(48)
  [47]=>
  int(49)
  [48]=>
  int(50)
  [49]=>
  int(51)
  [50]=>
  int(52)
  [51]=>
  int(53)
  [52]=>
  int(54)
  [53]=>
  int(55)
  [54]=>
  int(56)
  [55]=>
  int(57)
  [56]=>
  int(58)
  [57]=>
  int(59)
  [58]=>
  int(60)
  [59]=>
  int(61)
  [60]=>
  int(62)
  [61]=>
  int(63)
  [62]=>
  int(64)
  [63]=>
  int(65)
  [64]=>
  int(66)
  [65]=>
  int(67)
  [66]=>
  int(68)
  [67]=>
  int(69)
  [68]=>
  int(70)
  [69]=>
  int(71)
  [70]=>
  int(72)
  [71]=>
  int(73)
  [72]=>
  int(74)
  [73]=>
  int(75)
  [74]=>
  int(76)
  [75]=>
  int(77)
  [76]=>
  int(78)
  [77]=>
  int(79)
  [78]=>
  int(80)
  [79]=>
  int(81)
  [80]=>
  int(82)
  [81]=>
  int(83)
  [82]=>
  int(84)
  [83]=>
  int(85)
  [84]=>
  int(86)
  [85]=>
  int(87)
  [86]=>
  int(88)
  [87]=>
  int(89)
  [88]=>
  int(90)
  [89]=>
  int(91)
  [90]=>
  int(92)
  [91]=>
  int(93)
  [92]=>
  int(94)
  [93]=>
  int(95)
  [94]=>
  int(96)
  [95]=>
  int(97)
  [96]=>
  int(98)
  [97]=>
  int(99)
  [98]=>
  int(100)
  [99]=>
  int(101)
  [100]=>
  int(102)
  [101]=>
  int(103)
  [102]=>
  int(104)
  [103]=>
  int(105)
  [104]=>
  int(106)
  [105]=>
  int(107)
  [106]=>
  int(108)
  [107]=>
  int(109)
  [108]=>
  int(110)
  [109]=>
  int(111)
  [110]=>
  int(112)
  [111]=>
  int(113)
  [112]=>
  int(114)
  [113]=>
  int(115)
  [114]=>
  int(116)
  [115]=>
  int(117)
  [116]=>
  int(118)
  [117]=>
  int(119)
  [118]=>
  int(120)
  [119]=>
  int(121)
  [120]=>
  int(122)
  [121]=>
  int(123)
  [122]=>
  int(1)
}
array(1) {
  [0]=>
  int(0)
}
Failed to generate an acceptable random number in 50 attempts
string(6) "oobarf"
