--TEST--
Test array_walk, arsort, asort, krsort, ksort, rsort and sort
--POST--
--GET--
--FILE--
<?php
chdir(dirname(__FILE__));
/*
** Create sample arrays
** Test alpha, numeric (decimal, hex, octal) and special data
**
**
*/

/* Helper function to build testing arrays */
function make_nested_array ($depth, $breadth, $function = NULL, $args = array ()) {
    for ($x = 0; $x < $breadth; ++$x) {
        if (NULL === $function) {
            $array = array (0);
        } else {
            $array = array (call_user_func_array ($function, $args));
        }
        for ($y = 1; $y < $depth; ++$y) {
            $array[0] = array ($array[0]);
        }
        $temp[$x] = $array;
    }
    return $temp;
}

/* Standard numerical and associative arrays */
foreach (file ('array_data.txt') as $k => $v) {
    $v = trim ($v);
    $ref_array[0][$k] = $v;
    $ref_array[1][$v] = $k;
}

/* - Numeric and string indexes at and near the boundary of float */
for ($x = 63; $x < 66; ++$x) {
    $val = pow (2, $x);
    $ref_array[3][$val] = $x;
    $ref_array[3][-$val] = -$x;
    $ref_array[4]["$val"] = $x;
    $ref_array[4]["-$val"] = -$x;
}

/* - Numeric and string indexes at and near the boundary of int */
foreach (array (2147483646, 2147483647, 2147483648) as $x) {
    $ref_array[5][$x] = $x;
    $ref_array[5][ -$x] = -$x;
    $ref_array[6]["$x"] = $x;
    $ref_array[6]["-$x"] = -$x;
}

/* String index that contains a null */
/* XXX: Testing framework does not like nulls */
# $ref_array[7]["foo\0bar"] = 0;

/* Value that contains a null */
/* XXX: Testing framework does not like nulls */
# $ref_array[8][0] = "foo\0bar";

/* Empty array */
$ref_array[9] = array ();

/* Nested array */
$ref_array[10] = make_nested_array (5, 5);

var_dump ($ref_array);

function echo_kv ($value, $key) {
    var_dump ($key);
    var_dump ($value);
}

echo "\n -- Testing array_walk() -- \n";
foreach ($ref_array as $data) {
    array_walk ($data, 'echo_kv');
    var_dump ($data);
}

function test_sort ($sort_function, $test_data) {
    echo "\n -- Testing $sort_function() -- \n";
    foreach ($test_data as $data) {
        $sort_function ($data);
        var_dump ($data);
        $sort_function ($data, SORT_REGULAR);
        var_dump ($data);
        $sort_function ($data, SORT_NUMERIC);
        var_dump ($data);
        $sort_function ($data, SORT_STRING);
        var_dump ($data);
    }
}

foreach (array ('arsort', 'asort', 'krsort', 'ksort', 'rsort', 'sort') as $test_function) {
    test_sort ($test_function, $ref_array);
}

function cmp ($a, $b) {
    is_array ($a)
        and $a = array_sum ($a);
    is_array ($b)
        and $b = array_sum ($b);
    return strcmp ($a, $b);
}

echo "\n -- Testing uasort() -- \n";
foreach ($ref_array as $data) {
    uasort ($data, 'cmp');
    var_dump ($data);
}

echo "\n -- Testing uksort() -- \n";
foreach ($ref_array as $data) {
    uksort ($data, 'cmp');
    var_dump ($data);
}

echo "\n -- Testing usort() -- \n";
foreach ($ref_array as $data) {
    usort ($data, 'cmp');
    var_dump ($data);
}
?>
--EXPECT--
array(8) {
  [0]=>
  array(93) {
    [0]=>
    string(14) "Adam Dickmeiss"
    [1]=>
    string(11) "Alex Barkov"
    [2]=>
    string(11) "Alex Belits"
    [3]=>
    string(13) "Alex Plotnick"
    [4]=>
    string(17) "Alexander Aulbach"
    [5]=>
    string(17) "Alexander Feldman"
    [6]=>
    string(13) "Amitay Isaacs"
    [7]=>
    string(12) "Andi Gutmans"
    [8]=>
    string(15) "Andre Langhorst"
    [9]=>
    string(18) "Andreas Karajannis"
    [10]=>
    string(15) "Andrei Zmievski"
    [11]=>
    string(13) "Andrew Avdeev"
    [12]=>
    string(14) "Andrew Skalski"
    [13]=>
    string(12) "Andy Sautins"
    [14]=>
    string(19) "Antoni Pamies Olive"
    [15]=>
    string(11) "Boian Bonev"
    [16]=>
    string(18) "Brendan W. McAdams"
    [17]=>
    string(10) "Brian Wang"
    [18]=>
    string(13) "Chad Robinson"
    [19]=>
    string(16) "Chris Vandomelen"
    [20]=>
    string(16) "Christian Cartus"
    [21]=>
    string(15) "Chuck Hagenbuch"
    [22]=>
    string(14) "Colin Viebrock"
    [23]=>
    string(4) "cpdf"
    [24]=>
    string(5) "crack"
    [25]=>
    string(5) "ctype"
    [26]=>
    string(4) "CURL"
    [27]=>
    string(9) "CyberCash"
    [28]=>
    string(18) "Daniel Beulshausen"
    [29]=>
    string(17) "Daniel R Kalowsky"
    [30]=>
    string(11) "Danny Heijl"
    [31]=>
    string(11) "Dave Hayden"
    [32]=>
    string(12) "David Benson"
    [33]=>
    string(11) "David Croft"
    [34]=>
    string(14) "David Eriksson"
    [35]=>
    string(12) "David Hedbor"
    [36]=>
    string(11) "David Hénot"
    [37]=>
    string(11) "David Sklar"
    [38]=>
    string(14) "Derick Rethans"
    [39]=>
    string(12) "Doug DeJulio"
    [40]=>
    string(11) "Egon Schmid"
    [41]=>
    string(11) "Eric Warnke"
    [42]=>
    string(12) "Evan Klinger"
    [43]=>
    string(16) "Frank M. Kromann"
    [44]=>
    string(12) "Fredrik Ohrn"
    [45]=>
    string(14) "Gerrit Thomson"
    [46]=>
    string(11) "Harald Radi"
    [47]=>
    string(18) "Hartmut Holzgraefe"
    [48]=>
    string(16) "Hellekin O. Wolf"
    [49]=>
    string(12) "Hojtsy Gabor"
    [50]=>
    string(17) "Holger Zimmermann"
    [51]=>
    string(16) "Ilia Alshanetsky"
    [52]=>
    string(13) "Jalal Pushman"
    [53]=>
    string(11) "James Moore"
    [54]=>
    string(11) "Jan Borsodi"
    [55]=>
    string(13) "Jani Taskinen"
    [56]=>
    string(12) "Jason Greene"
    [57]=>
    string(25) "Jayakumar Muthukumarasamy"
    [58]=>
    string(12) "Jim Winstead"
    [59]=>
    string(10) "Joey Smith"
    [60]=>
    string(13) "John Donagher"
    [61]=>
    string(10) "Jouni Ahto"
    [62]=>
    string(16) "Kaj-Michael Lang"
    [63]=>
    string(17) "Kristian Köhntopp"
    [64]=>
    string(18) "Lars Torben Wilson"
    [65]=>
    string(11) "Mark Musone"
    [66]=>
    string(12) "Mitch Golden"
    [67]=>
    string(21) "Nikos Mavroyanopoulos"
    [68]=>
    string(15) "Olivier Cahagne"
    [69]=>
    string(13) "Phil Driscoll"
    [70]=>
    string(16) "Ramil Kalimullin"
    [71]=>
    string(14) "Rasmus Lerdorf"
    [72]=>
    string(9) "Rex Logan"
    [73]=>
    string(8) "Sam Ruby"
    [74]=>
    string(14) "Sascha Kettler"
    [75]=>
    string(15) "Sascha Schumann"
    [76]=>
    string(18) "Sebastian Bergmann"
    [77]=>
    string(17) "Sergey Kartashoff"
    [78]=>
    string(13) "Shane Caraveo"
    [79]=>
    string(14) "Slava Poliakov"
    [80]=>
    string(15) "Stefan Roehrich"
    [81]=>
    string(16) "Stephanie Wehner"
    [82]=>
    string(15) "Sterling Hughes"
    [83]=>
    string(11) "Stig Bakken"
    [84]=>
    string(11) "Stig Venaas"
    [85]=>
    string(16) "Thies C. Arntzen"
    [86]=>
    string(7) "Tom May"
    [87]=>
    string(14) "Tsukada Takuya"
    [88]=>
    string(13) "Uwe Steinmann"
    [89]=>
    string(11) "Vlad Krupin"
    [90]=>
    string(11) "Wez Furlong"
    [91]=>
    string(10) "Zak Greant"
    [92]=>
    string(12) "Zeev Suraski"
  }
  [1]=>
  array(93) {
    ["Adam Dickmeiss"]=>
    int(0)
    ["Alex Barkov"]=>
    int(1)
    ["Alex Belits"]=>
    int(2)
    ["Alex Plotnick"]=>
    int(3)
    ["Alexander Aulbach"]=>
    int(4)
    ["Alexander Feldman"]=>
    int(5)
    ["Amitay Isaacs"]=>
    int(6)
    ["Andi Gutmans"]=>
    int(7)
    ["Andre Langhorst"]=>
    int(8)
    ["Andreas Karajannis"]=>
    int(9)
    ["Andrei Zmievski"]=>
    int(10)
    ["Andrew Avdeev"]=>
    int(11)
    ["Andrew Skalski"]=>
    int(12)
    ["Andy Sautins"]=>
    int(13)
    ["Antoni Pamies Olive"]=>
    int(14)
    ["Boian Bonev"]=>
    int(15)
    ["Brendan W. McAdams"]=>
    int(16)
    ["Brian Wang"]=>
    int(17)
    ["Chad Robinson"]=>
    int(18)
    ["Chris Vandomelen"]=>
    int(19)
    ["Christian Cartus"]=>
    int(20)
    ["Chuck Hagenbuch"]=>
    int(21)
    ["Colin Viebrock"]=>
    int(22)
    ["cpdf"]=>
    int(23)
    ["crack"]=>
    int(24)
    ["ctype"]=>
    int(25)
    ["CURL"]=>
    int(26)
    ["CyberCash"]=>
    int(27)
    ["Daniel Beulshausen"]=>
    int(28)
    ["Daniel R Kalowsky"]=>
    int(29)
    ["Danny Heijl"]=>
    int(30)
    ["Dave Hayden"]=>
    int(31)
    ["David Benson"]=>
    int(32)
    ["David Croft"]=>
    int(33)
    ["David Eriksson"]=>
    int(34)
    ["David Hedbor"]=>
    int(35)
    ["David Hénot"]=>
    int(36)
    ["David Sklar"]=>
    int(37)
    ["Derick Rethans"]=>
    int(38)
    ["Doug DeJulio"]=>
    int(39)
    ["Egon Schmid"]=>
    int(40)
    ["Eric Warnke"]=>
    int(41)
    ["Evan Klinger"]=>
    int(42)
    ["Frank M. Kromann"]=>
    int(43)
    ["Fredrik Ohrn"]=>
    int(44)
    ["Gerrit Thomson"]=>
    int(45)
    ["Harald Radi"]=>
    int(46)
    ["Hartmut Holzgraefe"]=>
    int(47)
    ["Hellekin O. Wolf"]=>
    int(48)
    ["Hojtsy Gabor"]=>
    int(49)
    ["Holger Zimmermann"]=>
    int(50)
    ["Ilia Alshanetsky"]=>
    int(51)
    ["Jalal Pushman"]=>
    int(52)
    ["James Moore"]=>
    int(53)
    ["Jan Borsodi"]=>
    int(54)
    ["Jani Taskinen"]=>
    int(55)
    ["Jason Greene"]=>
    int(56)
    ["Jayakumar Muthukumarasamy"]=>
    int(57)
    ["Jim Winstead"]=>
    int(58)
    ["Joey Smith"]=>
    int(59)
    ["John Donagher"]=>
    int(60)
    ["Jouni Ahto"]=>
    int(61)
    ["Kaj-Michael Lang"]=>
    int(62)
    ["Kristian Köhntopp"]=>
    int(63)
    ["Lars Torben Wilson"]=>
    int(64)
    ["Mark Musone"]=>
    int(65)
    ["Mitch Golden"]=>
    int(66)
    ["Nikos Mavroyanopoulos"]=>
    int(67)
    ["Olivier Cahagne"]=>
    int(68)
    ["Phil Driscoll"]=>
    int(69)
    ["Ramil Kalimullin"]=>
    int(70)
    ["Rasmus Lerdorf"]=>
    int(71)
    ["Rex Logan"]=>
    int(72)
    ["Sam Ruby"]=>
    int(73)
    ["Sascha Kettler"]=>
    int(74)
    ["Sascha Schumann"]=>
    int(75)
    ["Sebastian Bergmann"]=>
    int(76)
    ["Sergey Kartashoff"]=>
    int(77)
    ["Shane Caraveo"]=>
    int(78)
    ["Slava Poliakov"]=>
    int(79)
    ["Stefan Roehrich"]=>
    int(80)
    ["Stephanie Wehner"]=>
    int(81)
    ["Sterling Hughes"]=>
    int(82)
    ["Stig Bakken"]=>
    int(83)
    ["Stig Venaas"]=>
    int(84)
    ["Thies C. Arntzen"]=>
    int(85)
    ["Tom May"]=>
    int(86)
    ["Tsukada Takuya"]=>
    int(87)
    ["Uwe Steinmann"]=>
    int(88)
    ["Vlad Krupin"]=>
    int(89)
    ["Wez Furlong"]=>
    int(90)
    ["Zak Greant"]=>
    int(91)
    ["Zeev Suraski"]=>
    int(92)
  }
  [3]=>
  array(1) {
    [-2147483648]=>
    int(-65)
  }
  [4]=>
  array(6) {
    ["9.2233720368547E+18"]=>
    int(63)
    ["-9.2233720368547E+18"]=>
    int(-63)
    ["1.844674407371E+19"]=>
    int(64)
    ["-1.844674407371E+19"]=>
    int(-64)
    ["3.6893488147419E+19"]=>
    int(65)
    ["-3.6893488147419E+19"]=>
    int(-65)
  }
  [5]=>
  array(5) {
    [2147483646]=>
    int(2147483646)
    [-2147483646]=>
    int(-2147483646)
    [2147483647]=>
    int(2147483647)
    [-2147483647]=>
    int(-2147483647)
    [-2147483648]=>
    float(-2147483648)
  }
  [6]=>
  array(6) {
    [2147483646]=>
    int(2147483646)
    ["-2147483646"]=>
    int(-2147483646)
    ["2147483647"]=>
    int(2147483647)
    ["-2147483647"]=>
    int(-2147483647)
    ["2147483648"]=>
    float(2147483648)
    ["-2147483648"]=>
    float(-2147483648)
  }
  [9]=>
  array(0) {
  }
  [10]=>
  array(5) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              int(0)
            }
          }
        }
      }
    }
    [1]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              int(0)
            }
          }
        }
      }
    }
    [2]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              int(0)
            }
          }
        }
      }
    }
    [3]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              int(0)
            }
          }
        }
      }
    }
    [4]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              int(0)
            }
          }
        }
      }
    }
  }
}

 -- Testing array_walk() -- 
int(0)
string(14) "Adam Dickmeiss"
int(1)
string(11) "Alex Barkov"
int(2)
string(11) "Alex Belits"
int(3)
string(13) "Alex Plotnick"
int(4)
string(17) "Alexander Aulbach"
int(5)
string(17) "Alexander Feldman"
int(6)
string(13) "Amitay Isaacs"
int(7)
string(12) "Andi Gutmans"
int(8)
string(15) "Andre Langhorst"
int(9)
string(18) "Andreas Karajannis"
int(10)
string(15) "Andrei Zmievski"
int(11)
string(13) "Andrew Avdeev"
int(12)
string(14) "Andrew Skalski"
int(13)
string(12) "Andy Sautins"
int(14)
string(19) "Antoni Pamies Olive"
int(15)
string(11) "Boian Bonev"
int(16)
string(18) "Brendan W. McAdams"
int(17)
string(10) "Brian Wang"
int(18)
string(13) "Chad Robinson"
int(19)
string(16) "Chris Vandomelen"
int(20)
string(16) "Christian Cartus"
int(21)
string(15) "Chuck Hagenbuch"
int(22)
string(14) "Colin Viebrock"
int(23)
string(4) "cpdf"
int(24)
string(5) "crack"
int(25)
string(5) "ctype"
int(26)
string(4) "CURL"
int(27)
string(9) "CyberCash"
int(28)
string(18) "Daniel Beulshausen"
int(29)
string(17) "Daniel R Kalowsky"
int(30)
string(11) "Danny Heijl"
int(31)
string(11) "Dave Hayden"
int(32)
string(12) "David Benson"
int(33)
string(11) "David Croft"
int(34)
string(14) "David Eriksson"
int(35)
string(12) "David Hedbor"
int(36)
string(11) "David Hénot"
int(37)
string(11) "David Sklar"
int(38)
string(14) "Derick Rethans"
int(39)
string(12) "Doug DeJulio"
int(40)
string(11) "Egon Schmid"
int(41)
string(11) "Eric Warnke"
int(42)
string(12) "Evan Klinger"
int(43)
string(16) "Frank M. Kromann"
int(44)
string(12) "Fredrik Ohrn"
int(45)
string(14) "Gerrit Thomson"
int(46)
string(11) "Harald Radi"
int(47)
string(18) "Hartmut Holzgraefe"
int(48)
string(16) "Hellekin O. Wolf"
int(49)
string(12) "Hojtsy Gabor"
int(50)
string(17) "Holger Zimmermann"
int(51)
string(16) "Ilia Alshanetsky"
int(52)
string(13) "Jalal Pushman"
int(53)
string(11) "James Moore"
int(54)
string(11) "Jan Borsodi"
int(55)
string(13) "Jani Taskinen"
int(56)
string(12) "Jason Greene"
int(57)
string(25) "Jayakumar Muthukumarasamy"
int(58)
string(12) "Jim Winstead"
int(59)
string(10) "Joey Smith"
int(60)
string(13) "John Donagher"
int(61)
string(10) "Jouni Ahto"
int(62)
string(16) "Kaj-Michael Lang"
int(63)
string(17) "Kristian Köhntopp"
int(64)
string(18) "Lars Torben Wilson"
int(65)
string(11) "Mark Musone"
int(66)
string(12) "Mitch Golden"
int(67)
string(21) "Nikos Mavroyanopoulos"
int(68)
string(15) "Olivier Cahagne"
int(69)
string(13) "Phil Driscoll"
int(70)
string(16) "Ramil Kalimullin"
int(71)
string(14) "Rasmus Lerdorf"
int(72)
string(9) "Rex Logan"
int(73)
string(8) "Sam Ruby"
int(74)
string(14) "Sascha Kettler"
int(75)
string(15) "Sascha Schumann"
int(76)
string(18) "Sebastian Bergmann"
int(77)
string(17) "Sergey Kartashoff"
int(78)
string(13) "Shane Caraveo"
int(79)
string(14) "Slava Poliakov"
int(80)
string(15) "Stefan Roehrich"
int(81)
string(16) "Stephanie Wehner"
int(82)
string(15) "Sterling Hughes"
int(83)
string(11) "Stig Bakken"
int(84)
string(11) "Stig Venaas"
int(85)
string(16) "Thies C. Arntzen"
int(86)
string(7) "Tom May"
int(87)
string(14) "Tsukada Takuya"
int(88)
string(13) "Uwe Steinmann"
int(89)
string(11) "Vlad Krupin"
int(90)
string(11) "Wez Furlong"
int(91)
string(10) "Zak Greant"
int(92)
string(12) "Zeev Suraski"
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
  [26]=>
  string(4) "CURL"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
}
string(14) "Adam Dickmeiss"
int(0)
string(11) "Alex Barkov"
int(1)
string(11) "Alex Belits"
int(2)
string(13) "Alex Plotnick"
int(3)
string(17) "Alexander Aulbach"
int(4)
string(17) "Alexander Feldman"
int(5)
string(13) "Amitay Isaacs"
int(6)
string(12) "Andi Gutmans"
int(7)
string(15) "Andre Langhorst"
int(8)
string(18) "Andreas Karajannis"
int(9)
string(15) "Andrei Zmievski"
int(10)
string(13) "Andrew Avdeev"
int(11)
string(14) "Andrew Skalski"
int(12)
string(12) "Andy Sautins"
int(13)
string(19) "Antoni Pamies Olive"
int(14)
string(11) "Boian Bonev"
int(15)
string(18) "Brendan W. McAdams"
int(16)
string(10) "Brian Wang"
int(17)
string(13) "Chad Robinson"
int(18)
string(16) "Chris Vandomelen"
int(19)
string(16) "Christian Cartus"
int(20)
string(15) "Chuck Hagenbuch"
int(21)
string(14) "Colin Viebrock"
int(22)
string(4) "cpdf"
int(23)
string(5) "crack"
int(24)
string(5) "ctype"
int(25)
string(4) "CURL"
int(26)
string(9) "CyberCash"
int(27)
string(18) "Daniel Beulshausen"
int(28)
string(17) "Daniel R Kalowsky"
int(29)
string(11) "Danny Heijl"
int(30)
string(11) "Dave Hayden"
int(31)
string(12) "David Benson"
int(32)
string(11) "David Croft"
int(33)
string(14) "David Eriksson"
int(34)
string(12) "David Hedbor"
int(35)
string(11) "David Hénot"
int(36)
string(11) "David Sklar"
int(37)
string(14) "Derick Rethans"
int(38)
string(12) "Doug DeJulio"
int(39)
string(11) "Egon Schmid"
int(40)
string(11) "Eric Warnke"
int(41)
string(12) "Evan Klinger"
int(42)
string(16) "Frank M. Kromann"
int(43)
string(12) "Fredrik Ohrn"
int(44)
string(14) "Gerrit Thomson"
int(45)
string(11) "Harald Radi"
int(46)
string(18) "Hartmut Holzgraefe"
int(47)
string(16) "Hellekin O. Wolf"
int(48)
string(12) "Hojtsy Gabor"
int(49)
string(17) "Holger Zimmermann"
int(50)
string(16) "Ilia Alshanetsky"
int(51)
string(13) "Jalal Pushman"
int(52)
string(11) "James Moore"
int(53)
string(11) "Jan Borsodi"
int(54)
string(13) "Jani Taskinen"
int(55)
string(12) "Jason Greene"
int(56)
string(25) "Jayakumar Muthukumarasamy"
int(57)
string(12) "Jim Winstead"
int(58)
string(10) "Joey Smith"
int(59)
string(13) "John Donagher"
int(60)
string(10) "Jouni Ahto"
int(61)
string(16) "Kaj-Michael Lang"
int(62)
string(17) "Kristian Köhntopp"
int(63)
string(18) "Lars Torben Wilson"
int(64)
string(11) "Mark Musone"
int(65)
string(12) "Mitch Golden"
int(66)
string(21) "Nikos Mavroyanopoulos"
int(67)
string(15) "Olivier Cahagne"
int(68)
string(13) "Phil Driscoll"
int(69)
string(16) "Ramil Kalimullin"
int(70)
string(14) "Rasmus Lerdorf"
int(71)
string(9) "Rex Logan"
int(72)
string(8) "Sam Ruby"
int(73)
string(14) "Sascha Kettler"
int(74)
string(15) "Sascha Schumann"
int(75)
string(18) "Sebastian Bergmann"
int(76)
string(17) "Sergey Kartashoff"
int(77)
string(13) "Shane Caraveo"
int(78)
string(14) "Slava Poliakov"
int(79)
string(15) "Stefan Roehrich"
int(80)
string(16) "Stephanie Wehner"
int(81)
string(15) "Sterling Hughes"
int(82)
string(11) "Stig Bakken"
int(83)
string(11) "Stig Venaas"
int(84)
string(16) "Thies C. Arntzen"
int(85)
string(7) "Tom May"
int(86)
string(14) "Tsukada Takuya"
int(87)
string(13) "Uwe Steinmann"
int(88)
string(11) "Vlad Krupin"
int(89)
string(11) "Wez Furlong"
int(90)
string(10) "Zak Greant"
int(91)
string(12) "Zeev Suraski"
int(92)
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
  ["CURL"]=>
  int(26)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
}
int(-2147483648)
int(-65)
array(1) {
  [-2147483648]=>
  int(-65)
}
string(19) "9.2233720368547E+18"
int(63)
string(20) "-9.2233720368547E+18"
int(-63)
string(18) "1.844674407371E+19"
int(64)
string(19) "-1.844674407371E+19"
int(-64)
string(19) "3.6893488147419E+19"
int(65)
string(20) "-3.6893488147419E+19"
int(-65)
array(6) {
  ["9.2233720368547E+18"]=>
  int(63)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["1.844674407371E+19"]=>
  int(64)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["3.6893488147419E+19"]=>
  int(65)
  ["-3.6893488147419E+19"]=>
  int(-65)
}
int(2147483646)
int(2147483646)
int(-2147483646)
int(-2147483646)
int(2147483647)
int(2147483647)
int(-2147483647)
int(-2147483647)
int(-2147483648)
float(-2147483648)
array(5) {
  [2147483646]=>
  int(2147483646)
  [-2147483646]=>
  int(-2147483646)
  [2147483647]=>
  int(2147483647)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
}
int(2147483646)
int(2147483646)
string(11) "-2147483646"
int(-2147483646)
string(10) "2147483647"
int(2147483647)
string(11) "-2147483647"
int(-2147483647)
string(10) "2147483648"
float(2147483648)
string(11) "-2147483648"
float(-2147483648)
array(6) {
  [2147483646]=>
  int(2147483646)
  ["-2147483646"]=>
  int(-2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["-2147483647"]=>
  int(-2147483647)
  ["2147483648"]=>
  float(2147483648)
  ["-2147483648"]=>
  float(-2147483648)
}
array(0) {
}
int(0)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          int(0)
        }
      }
    }
  }
}
int(1)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          int(0)
        }
      }
    }
  }
}
int(2)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          int(0)
        }
      }
    }
  }
}
int(3)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          int(0)
        }
      }
    }
  }
}
int(4)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          int(0)
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing arsort() -- 
array(93) {
  [25]=>
  string(5) "ctype"
  [24]=>
  string(5) "crack"
  [23]=>
  string(4) "cpdf"
  [92]=>
  string(12) "Zeev Suraski"
  [91]=>
  string(10) "Zak Greant"
  [90]=>
  string(11) "Wez Furlong"
  [89]=>
  string(11) "Vlad Krupin"
  [88]=>
  string(13) "Uwe Steinmann"
  [87]=>
  string(14) "Tsukada Takuya"
  [86]=>
  string(7) "Tom May"
  [85]=>
  string(16) "Thies C. Arntzen"
  [84]=>
  string(11) "Stig Venaas"
  [83]=>
  string(11) "Stig Bakken"
  [82]=>
  string(15) "Sterling Hughes"
  [81]=>
  string(16) "Stephanie Wehner"
  [80]=>
  string(15) "Stefan Roehrich"
  [79]=>
  string(14) "Slava Poliakov"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [76]=>
  string(18) "Sebastian Bergmann"
  [75]=>
  string(15) "Sascha Schumann"
  [74]=>
  string(14) "Sascha Kettler"
  [73]=>
  string(8) "Sam Ruby"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [70]=>
  string(16) "Ramil Kalimullin"
  [69]=>
  string(13) "Phil Driscoll"
  [68]=>
  string(15) "Olivier Cahagne"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [64]=>
  string(18) "Lars Torben Wilson"
  [63]=>
  string(17) "Kristian Köhntopp"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [61]=>
  string(10) "Jouni Ahto"
  [60]=>
  string(13) "John Donagher"
  [59]=>
  string(10) "Joey Smith"
  [58]=>
  string(12) "Jim Winstead"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [56]=>
  string(12) "Jason Greene"
  [55]=>
  string(13) "Jani Taskinen"
  [54]=>
  string(11) "Jan Borsodi"
  [53]=>
  string(11) "James Moore"
  [52]=>
  string(13) "Jalal Pushman"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [50]=>
  string(17) "Holger Zimmermann"
  [49]=>
  string(12) "Hojtsy Gabor"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [46]=>
  string(11) "Harald Radi"
  [45]=>
  string(14) "Gerrit Thomson"
  [44]=>
  string(12) "Fredrik Ohrn"
  [43]=>
  string(16) "Frank M. Kromann"
  [42]=>
  string(12) "Evan Klinger"
  [41]=>
  string(11) "Eric Warnke"
  [40]=>
  string(11) "Egon Schmid"
  [39]=>
  string(12) "Doug DeJulio"
  [38]=>
  string(14) "Derick Rethans"
  [37]=>
  string(11) "David Sklar"
  [36]=>
  string(11) "David Hénot"
  [35]=>
  string(12) "David Hedbor"
  [34]=>
  string(14) "David Eriksson"
  [33]=>
  string(11) "David Croft"
  [32]=>
  string(12) "David Benson"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [28]=>
  string(18) "Daniel Beulshausen"
  [27]=>
  string(9) "CyberCash"
  [22]=>
  string(14) "Colin Viebrock"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [19]=>
  string(16) "Chris Vandomelen"
  [18]=>
  string(13) "Chad Robinson"
  [26]=>
  string(4) "CURL"
  [17]=>
  string(10) "Brian Wang"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [13]=>
  string(12) "Andy Sautins"
  [12]=>
  string(14) "Andrew Skalski"
  [11]=>
  string(13) "Andrew Avdeev"
  [10]=>
  string(15) "Andrei Zmievski"
  [9]=>
  string(18) "Andreas Karajannis"
  [8]=>
  string(15) "Andre Langhorst"
  [7]=>
  string(12) "Andi Gutmans"
  [6]=>
  string(13) "Amitay Isaacs"
  [5]=>
  string(17) "Alexander Feldman"
  [4]=>
  string(17) "Alexander Aulbach"
  [3]=>
  string(13) "Alex Plotnick"
  [2]=>
  string(11) "Alex Belits"
  [1]=>
  string(11) "Alex Barkov"
  [0]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [25]=>
  string(5) "ctype"
  [24]=>
  string(5) "crack"
  [23]=>
  string(4) "cpdf"
  [92]=>
  string(12) "Zeev Suraski"
  [91]=>
  string(10) "Zak Greant"
  [90]=>
  string(11) "Wez Furlong"
  [89]=>
  string(11) "Vlad Krupin"
  [88]=>
  string(13) "Uwe Steinmann"
  [87]=>
  string(14) "Tsukada Takuya"
  [86]=>
  string(7) "Tom May"
  [85]=>
  string(16) "Thies C. Arntzen"
  [84]=>
  string(11) "Stig Venaas"
  [83]=>
  string(11) "Stig Bakken"
  [82]=>
  string(15) "Sterling Hughes"
  [81]=>
  string(16) "Stephanie Wehner"
  [80]=>
  string(15) "Stefan Roehrich"
  [79]=>
  string(14) "Slava Poliakov"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [76]=>
  string(18) "Sebastian Bergmann"
  [75]=>
  string(15) "Sascha Schumann"
  [74]=>
  string(14) "Sascha Kettler"
  [73]=>
  string(8) "Sam Ruby"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [70]=>
  string(16) "Ramil Kalimullin"
  [69]=>
  string(13) "Phil Driscoll"
  [68]=>
  string(15) "Olivier Cahagne"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [64]=>
  string(18) "Lars Torben Wilson"
  [63]=>
  string(17) "Kristian Köhntopp"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [61]=>
  string(10) "Jouni Ahto"
  [60]=>
  string(13) "John Donagher"
  [59]=>
  string(10) "Joey Smith"
  [58]=>
  string(12) "Jim Winstead"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [56]=>
  string(12) "Jason Greene"
  [55]=>
  string(13) "Jani Taskinen"
  [54]=>
  string(11) "Jan Borsodi"
  [53]=>
  string(11) "James Moore"
  [52]=>
  string(13) "Jalal Pushman"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [50]=>
  string(17) "Holger Zimmermann"
  [49]=>
  string(12) "Hojtsy Gabor"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [46]=>
  string(11) "Harald Radi"
  [45]=>
  string(14) "Gerrit Thomson"
  [44]=>
  string(12) "Fredrik Ohrn"
  [43]=>
  string(16) "Frank M. Kromann"
  [42]=>
  string(12) "Evan Klinger"
  [41]=>
  string(11) "Eric Warnke"
  [40]=>
  string(11) "Egon Schmid"
  [39]=>
  string(12) "Doug DeJulio"
  [38]=>
  string(14) "Derick Rethans"
  [37]=>
  string(11) "David Sklar"
  [36]=>
  string(11) "David Hénot"
  [35]=>
  string(12) "David Hedbor"
  [34]=>
  string(14) "David Eriksson"
  [33]=>
  string(11) "David Croft"
  [32]=>
  string(12) "David Benson"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [28]=>
  string(18) "Daniel Beulshausen"
  [27]=>
  string(9) "CyberCash"
  [22]=>
  string(14) "Colin Viebrock"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [19]=>
  string(16) "Chris Vandomelen"
  [18]=>
  string(13) "Chad Robinson"
  [26]=>
  string(4) "CURL"
  [17]=>
  string(10) "Brian Wang"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [13]=>
  string(12) "Andy Sautins"
  [12]=>
  string(14) "Andrew Skalski"
  [11]=>
  string(13) "Andrew Avdeev"
  [10]=>
  string(15) "Andrei Zmievski"
  [9]=>
  string(18) "Andreas Karajannis"
  [8]=>
  string(15) "Andre Langhorst"
  [7]=>
  string(12) "Andi Gutmans"
  [6]=>
  string(13) "Amitay Isaacs"
  [5]=>
  string(17) "Alexander Feldman"
  [4]=>
  string(17) "Alexander Aulbach"
  [3]=>
  string(13) "Alex Plotnick"
  [2]=>
  string(11) "Alex Belits"
  [1]=>
  string(11) "Alex Barkov"
  [0]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [22]=>
  string(14) "Colin Viebrock"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [43]=>
  string(16) "Frank M. Kromann"
  [42]=>
  string(12) "Evan Klinger"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [5]=>
  string(17) "Alexander Feldman"
  [4]=>
  string(17) "Alexander Aulbach"
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [17]=>
  string(10) "Brian Wang"
  [26]=>
  string(4) "CURL"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [90]=>
  string(11) "Wez Furlong"
  [89]=>
  string(11) "Vlad Krupin"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [55]=>
  string(13) "Jani Taskinen"
  [54]=>
  string(11) "Jan Borsodi"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [25]=>
  string(5) "ctype"
}
array(93) {
  [25]=>
  string(5) "ctype"
  [24]=>
  string(5) "crack"
  [23]=>
  string(4) "cpdf"
  [92]=>
  string(12) "Zeev Suraski"
  [91]=>
  string(10) "Zak Greant"
  [90]=>
  string(11) "Wez Furlong"
  [89]=>
  string(11) "Vlad Krupin"
  [88]=>
  string(13) "Uwe Steinmann"
  [87]=>
  string(14) "Tsukada Takuya"
  [86]=>
  string(7) "Tom May"
  [85]=>
  string(16) "Thies C. Arntzen"
  [84]=>
  string(11) "Stig Venaas"
  [83]=>
  string(11) "Stig Bakken"
  [82]=>
  string(15) "Sterling Hughes"
  [81]=>
  string(16) "Stephanie Wehner"
  [80]=>
  string(15) "Stefan Roehrich"
  [79]=>
  string(14) "Slava Poliakov"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [76]=>
  string(18) "Sebastian Bergmann"
  [75]=>
  string(15) "Sascha Schumann"
  [74]=>
  string(14) "Sascha Kettler"
  [73]=>
  string(8) "Sam Ruby"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [70]=>
  string(16) "Ramil Kalimullin"
  [69]=>
  string(13) "Phil Driscoll"
  [68]=>
  string(15) "Olivier Cahagne"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [64]=>
  string(18) "Lars Torben Wilson"
  [63]=>
  string(17) "Kristian Köhntopp"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [61]=>
  string(10) "Jouni Ahto"
  [60]=>
  string(13) "John Donagher"
  [59]=>
  string(10) "Joey Smith"
  [58]=>
  string(12) "Jim Winstead"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [56]=>
  string(12) "Jason Greene"
  [55]=>
  string(13) "Jani Taskinen"
  [54]=>
  string(11) "Jan Borsodi"
  [53]=>
  string(11) "James Moore"
  [52]=>
  string(13) "Jalal Pushman"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [50]=>
  string(17) "Holger Zimmermann"
  [49]=>
  string(12) "Hojtsy Gabor"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [46]=>
  string(11) "Harald Radi"
  [45]=>
  string(14) "Gerrit Thomson"
  [44]=>
  string(12) "Fredrik Ohrn"
  [43]=>
  string(16) "Frank M. Kromann"
  [42]=>
  string(12) "Evan Klinger"
  [41]=>
  string(11) "Eric Warnke"
  [40]=>
  string(11) "Egon Schmid"
  [39]=>
  string(12) "Doug DeJulio"
  [38]=>
  string(14) "Derick Rethans"
  [37]=>
  string(11) "David Sklar"
  [36]=>
  string(11) "David Hénot"
  [35]=>
  string(12) "David Hedbor"
  [34]=>
  string(14) "David Eriksson"
  [33]=>
  string(11) "David Croft"
  [32]=>
  string(12) "David Benson"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [28]=>
  string(18) "Daniel Beulshausen"
  [27]=>
  string(9) "CyberCash"
  [22]=>
  string(14) "Colin Viebrock"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [19]=>
  string(16) "Chris Vandomelen"
  [18]=>
  string(13) "Chad Robinson"
  [26]=>
  string(4) "CURL"
  [17]=>
  string(10) "Brian Wang"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [13]=>
  string(12) "Andy Sautins"
  [12]=>
  string(14) "Andrew Skalski"
  [11]=>
  string(13) "Andrew Avdeev"
  [10]=>
  string(15) "Andrei Zmievski"
  [9]=>
  string(18) "Andreas Karajannis"
  [8]=>
  string(15) "Andre Langhorst"
  [7]=>
  string(12) "Andi Gutmans"
  [6]=>
  string(13) "Amitay Isaacs"
  [5]=>
  string(17) "Alexander Feldman"
  [4]=>
  string(17) "Alexander Aulbach"
  [3]=>
  string(13) "Alex Plotnick"
  [2]=>
  string(11) "Alex Belits"
  [1]=>
  string(11) "Alex Barkov"
  [0]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  ["Zeev Suraski"]=>
  int(92)
  ["Zak Greant"]=>
  int(91)
  ["Wez Furlong"]=>
  int(90)
  ["Vlad Krupin"]=>
  int(89)
  ["Uwe Steinmann"]=>
  int(88)
  ["Tsukada Takuya"]=>
  int(87)
  ["Tom May"]=>
  int(86)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Stig Venaas"]=>
  int(84)
  ["Stig Bakken"]=>
  int(83)
  ["Sterling Hughes"]=>
  int(82)
  ["Stephanie Wehner"]=>
  int(81)
  ["Stefan Roehrich"]=>
  int(80)
  ["Slava Poliakov"]=>
  int(79)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sascha Schumann"]=>
  int(75)
  ["Sascha Kettler"]=>
  int(74)
  ["Sam Ruby"]=>
  int(73)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Phil Driscoll"]=>
  int(69)
  ["Olivier Cahagne"]=>
  int(68)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Jouni Ahto"]=>
  int(61)
  ["John Donagher"]=>
  int(60)
  ["Joey Smith"]=>
  int(59)
  ["Jim Winstead"]=>
  int(58)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jason Greene"]=>
  int(56)
  ["Jani Taskinen"]=>
  int(55)
  ["Jan Borsodi"]=>
  int(54)
  ["James Moore"]=>
  int(53)
  ["Jalal Pushman"]=>
  int(52)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Holger Zimmermann"]=>
  int(50)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Harald Radi"]=>
  int(46)
  ["Gerrit Thomson"]=>
  int(45)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Frank M. Kromann"]=>
  int(43)
  ["Evan Klinger"]=>
  int(42)
  ["Eric Warnke"]=>
  int(41)
  ["Egon Schmid"]=>
  int(40)
  ["Doug DeJulio"]=>
  int(39)
  ["Derick Rethans"]=>
  int(38)
  ["David Sklar"]=>
  int(37)
  ["David Hénot"]=>
  int(36)
  ["David Hedbor"]=>
  int(35)
  ["David Eriksson"]=>
  int(34)
  ["David Croft"]=>
  int(33)
  ["David Benson"]=>
  int(32)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Daniel Beulshausen"]=>
  int(28)
  ["CyberCash"]=>
  int(27)
  ["CURL"]=>
  int(26)
  ["ctype"]=>
  int(25)
  ["crack"]=>
  int(24)
  ["cpdf"]=>
  int(23)
  ["Colin Viebrock"]=>
  int(22)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Chris Vandomelen"]=>
  int(19)
  ["Chad Robinson"]=>
  int(18)
  ["Brian Wang"]=>
  int(17)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Andy Sautins"]=>
  int(13)
  ["Andrew Skalski"]=>
  int(12)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andre Langhorst"]=>
  int(8)
  ["Andi Gutmans"]=>
  int(7)
  ["Amitay Isaacs"]=>
  int(6)
  ["Alexander Feldman"]=>
  int(5)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alex Plotnick"]=>
  int(3)
  ["Alex Belits"]=>
  int(2)
  ["Alex Barkov"]=>
  int(1)
  ["Adam Dickmeiss"]=>
  int(0)
}
array(93) {
  ["Zeev Suraski"]=>
  int(92)
  ["Zak Greant"]=>
  int(91)
  ["Wez Furlong"]=>
  int(90)
  ["Vlad Krupin"]=>
  int(89)
  ["Uwe Steinmann"]=>
  int(88)
  ["Tsukada Takuya"]=>
  int(87)
  ["Tom May"]=>
  int(86)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Stig Venaas"]=>
  int(84)
  ["Stig Bakken"]=>
  int(83)
  ["Sterling Hughes"]=>
  int(82)
  ["Stephanie Wehner"]=>
  int(81)
  ["Stefan Roehrich"]=>
  int(80)
  ["Slava Poliakov"]=>
  int(79)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sascha Schumann"]=>
  int(75)
  ["Sascha Kettler"]=>
  int(74)
  ["Sam Ruby"]=>
  int(73)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Phil Driscoll"]=>
  int(69)
  ["Olivier Cahagne"]=>
  int(68)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Jouni Ahto"]=>
  int(61)
  ["John Donagher"]=>
  int(60)
  ["Joey Smith"]=>
  int(59)
  ["Jim Winstead"]=>
  int(58)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jason Greene"]=>
  int(56)
  ["Jani Taskinen"]=>
  int(55)
  ["Jan Borsodi"]=>
  int(54)
  ["James Moore"]=>
  int(53)
  ["Jalal Pushman"]=>
  int(52)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Holger Zimmermann"]=>
  int(50)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Harald Radi"]=>
  int(46)
  ["Gerrit Thomson"]=>
  int(45)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Frank M. Kromann"]=>
  int(43)
  ["Evan Klinger"]=>
  int(42)
  ["Eric Warnke"]=>
  int(41)
  ["Egon Schmid"]=>
  int(40)
  ["Doug DeJulio"]=>
  int(39)
  ["Derick Rethans"]=>
  int(38)
  ["David Sklar"]=>
  int(37)
  ["David Hénot"]=>
  int(36)
  ["David Hedbor"]=>
  int(35)
  ["David Eriksson"]=>
  int(34)
  ["David Croft"]=>
  int(33)
  ["David Benson"]=>
  int(32)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Daniel Beulshausen"]=>
  int(28)
  ["CyberCash"]=>
  int(27)
  ["CURL"]=>
  int(26)
  ["ctype"]=>
  int(25)
  ["crack"]=>
  int(24)
  ["cpdf"]=>
  int(23)
  ["Colin Viebrock"]=>
  int(22)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Chris Vandomelen"]=>
  int(19)
  ["Chad Robinson"]=>
  int(18)
  ["Brian Wang"]=>
  int(17)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Andy Sautins"]=>
  int(13)
  ["Andrew Skalski"]=>
  int(12)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andre Langhorst"]=>
  int(8)
  ["Andi Gutmans"]=>
  int(7)
  ["Amitay Isaacs"]=>
  int(6)
  ["Alexander Feldman"]=>
  int(5)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alex Plotnick"]=>
  int(3)
  ["Alex Belits"]=>
  int(2)
  ["Alex Barkov"]=>
  int(1)
  ["Adam Dickmeiss"]=>
  int(0)
}
array(93) {
  ["Zeev Suraski"]=>
  int(92)
  ["Zak Greant"]=>
  int(91)
  ["Wez Furlong"]=>
  int(90)
  ["Vlad Krupin"]=>
  int(89)
  ["Uwe Steinmann"]=>
  int(88)
  ["Tsukada Takuya"]=>
  int(87)
  ["Tom May"]=>
  int(86)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Stig Venaas"]=>
  int(84)
  ["Stig Bakken"]=>
  int(83)
  ["Sterling Hughes"]=>
  int(82)
  ["Stephanie Wehner"]=>
  int(81)
  ["Stefan Roehrich"]=>
  int(80)
  ["Slava Poliakov"]=>
  int(79)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sascha Schumann"]=>
  int(75)
  ["Sascha Kettler"]=>
  int(74)
  ["Sam Ruby"]=>
  int(73)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Phil Driscoll"]=>
  int(69)
  ["Olivier Cahagne"]=>
  int(68)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Jouni Ahto"]=>
  int(61)
  ["John Donagher"]=>
  int(60)
  ["Joey Smith"]=>
  int(59)
  ["Jim Winstead"]=>
  int(58)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jason Greene"]=>
  int(56)
  ["Jani Taskinen"]=>
  int(55)
  ["Jan Borsodi"]=>
  int(54)
  ["James Moore"]=>
  int(53)
  ["Jalal Pushman"]=>
  int(52)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Holger Zimmermann"]=>
  int(50)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Harald Radi"]=>
  int(46)
  ["Gerrit Thomson"]=>
  int(45)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Frank M. Kromann"]=>
  int(43)
  ["Evan Klinger"]=>
  int(42)
  ["Eric Warnke"]=>
  int(41)
  ["Egon Schmid"]=>
  int(40)
  ["Doug DeJulio"]=>
  int(39)
  ["Derick Rethans"]=>
  int(38)
  ["David Sklar"]=>
  int(37)
  ["David Hénot"]=>
  int(36)
  ["David Hedbor"]=>
  int(35)
  ["David Eriksson"]=>
  int(34)
  ["David Croft"]=>
  int(33)
  ["David Benson"]=>
  int(32)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Daniel Beulshausen"]=>
  int(28)
  ["CyberCash"]=>
  int(27)
  ["CURL"]=>
  int(26)
  ["ctype"]=>
  int(25)
  ["crack"]=>
  int(24)
  ["cpdf"]=>
  int(23)
  ["Colin Viebrock"]=>
  int(22)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Chris Vandomelen"]=>
  int(19)
  ["Chad Robinson"]=>
  int(18)
  ["Brian Wang"]=>
  int(17)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Andy Sautins"]=>
  int(13)
  ["Andrew Skalski"]=>
  int(12)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andre Langhorst"]=>
  int(8)
  ["Andi Gutmans"]=>
  int(7)
  ["Amitay Isaacs"]=>
  int(6)
  ["Alexander Feldman"]=>
  int(5)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alex Plotnick"]=>
  int(3)
  ["Alex Belits"]=>
  int(2)
  ["Alex Barkov"]=>
  int(1)
  ["Adam Dickmeiss"]=>
  int(0)
}
array(93) {
  ["Zeev Suraski"]=>
  int(92)
  ["Zak Greant"]=>
  int(91)
  ["Wez Furlong"]=>
  int(90)
  ["Andreas Karajannis"]=>
  int(9)
  ["Vlad Krupin"]=>
  int(89)
  ["Uwe Steinmann"]=>
  int(88)
  ["Tsukada Takuya"]=>
  int(87)
  ["Tom May"]=>
  int(86)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Stig Venaas"]=>
  int(84)
  ["Stig Bakken"]=>
  int(83)
  ["Sterling Hughes"]=>
  int(82)
  ["Stephanie Wehner"]=>
  int(81)
  ["Stefan Roehrich"]=>
  int(80)
  ["Andre Langhorst"]=>
  int(8)
  ["Slava Poliakov"]=>
  int(79)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sascha Schumann"]=>
  int(75)
  ["Sascha Kettler"]=>
  int(74)
  ["Sam Ruby"]=>
  int(73)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Andi Gutmans"]=>
  int(7)
  ["Phil Driscoll"]=>
  int(69)
  ["Olivier Cahagne"]=>
  int(68)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Jouni Ahto"]=>
  int(61)
  ["John Donagher"]=>
  int(60)
  ["Amitay Isaacs"]=>
  int(6)
  ["Joey Smith"]=>
  int(59)
  ["Jim Winstead"]=>
  int(58)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jason Greene"]=>
  int(56)
  ["Jani Taskinen"]=>
  int(55)
  ["Jan Borsodi"]=>
  int(54)
  ["James Moore"]=>
  int(53)
  ["Jalal Pushman"]=>
  int(52)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Holger Zimmermann"]=>
  int(50)
  ["Alexander Feldman"]=>
  int(5)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Harald Radi"]=>
  int(46)
  ["Gerrit Thomson"]=>
  int(45)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Frank M. Kromann"]=>
  int(43)
  ["Evan Klinger"]=>
  int(42)
  ["Eric Warnke"]=>
  int(41)
  ["Egon Schmid"]=>
  int(40)
  ["Alexander Aulbach"]=>
  int(4)
  ["Doug DeJulio"]=>
  int(39)
  ["Derick Rethans"]=>
  int(38)
  ["David Sklar"]=>
  int(37)
  ["David Hénot"]=>
  int(36)
  ["David Hedbor"]=>
  int(35)
  ["David Eriksson"]=>
  int(34)
  ["David Croft"]=>
  int(33)
  ["David Benson"]=>
  int(32)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Alex Plotnick"]=>
  int(3)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Daniel Beulshausen"]=>
  int(28)
  ["CyberCash"]=>
  int(27)
  ["CURL"]=>
  int(26)
  ["ctype"]=>
  int(25)
  ["crack"]=>
  int(24)
  ["cpdf"]=>
  int(23)
  ["Colin Viebrock"]=>
  int(22)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Alex Belits"]=>
  int(2)
  ["Chris Vandomelen"]=>
  int(19)
  ["Chad Robinson"]=>
  int(18)
  ["Brian Wang"]=>
  int(17)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Andy Sautins"]=>
  int(13)
  ["Andrew Skalski"]=>
  int(12)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrei Zmievski"]=>
  int(10)
  ["Alex Barkov"]=>
  int(1)
  ["Adam Dickmeiss"]=>
  int(0)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  ["3.6893488147419E+19"]=>
  int(65)
  ["1.844674407371E+19"]=>
  int(64)
  ["9.2233720368547E+18"]=>
  int(63)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
}
array(6) {
  ["3.6893488147419E+19"]=>
  int(65)
  ["1.844674407371E+19"]=>
  int(64)
  ["9.2233720368547E+18"]=>
  int(63)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
}
array(6) {
  ["3.6893488147419E+19"]=>
  int(65)
  ["1.844674407371E+19"]=>
  int(64)
  ["9.2233720368547E+18"]=>
  int(63)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
}
array(6) {
  ["3.6893488147419E+19"]=>
  int(65)
  ["1.844674407371E+19"]=>
  int(64)
  ["9.2233720368547E+18"]=>
  int(63)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-9.2233720368547E+18"]=>
  int(-63)
}
array(5) {
  [2147483647]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
}
array(5) {
  [2147483647]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
}
array(5) {
  [2147483647]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
}
array(5) {
  [2147483647]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  [-2147483648]=>
  float(-2147483648)
  [-2147483647]=>
  int(-2147483647)
  [-2147483646]=>
  int(-2147483646)
}
array(6) {
  ["2147483648"]=>
  float(2147483648)
  ["2147483647"]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
}
array(6) {
  ["2147483648"]=>
  float(2147483648)
  ["2147483647"]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
}
array(6) {
  ["2147483648"]=>
  float(2147483648)
  ["2147483647"]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
}
array(6) {
  ["2147483648"]=>
  float(2147483648)
  ["2147483647"]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  ["-2147483648"]=>
  float(-2147483648)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483646"]=>
  int(-2147483646)
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing asort() -- 
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [26]=>
  string(4) "CURL"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
}
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [26]=>
  string(4) "CURL"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
}
array(93) {
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [64]=>
  string(18) "Lars Torben Wilson"
  [63]=>
  string(17) "Kristian Köhntopp"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [70]=>
  string(16) "Ramil Kalimullin"
  [69]=>
  string(13) "Phil Driscoll"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [61]=>
  string(10) "Jouni Ahto"
  [54]=>
  string(11) "Jan Borsodi"
  [53]=>
  string(11) "James Moore"
  [52]=>
  string(13) "Jalal Pushman"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [60]=>
  string(13) "John Donagher"
  [59]=>
  string(10) "Joey Smith"
  [58]=>
  string(12) "Jim Winstead"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [89]=>
  string(11) "Vlad Krupin"
  [88]=>
  string(13) "Uwe Steinmann"
  [87]=>
  string(14) "Tsukada Takuya"
  [86]=>
  string(7) "Tom May"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [25]=>
  string(5) "ctype"
  [24]=>
  string(5) "crack"
  [23]=>
  string(4) "cpdf"
  [92]=>
  string(12) "Zeev Suraski"
  [85]=>
  string(16) "Thies C. Arntzen"
  [84]=>
  string(11) "Stig Venaas"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [76]=>
  string(18) "Sebastian Bergmann"
  [75]=>
  string(15) "Sascha Schumann"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [83]=>
  string(11) "Stig Bakken"
  [82]=>
  string(15) "Sterling Hughes"
  [81]=>
  string(16) "Stephanie Wehner"
  [50]=>
  string(17) "Holger Zimmermann"
  [49]=>
  string(12) "Hojtsy Gabor"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [13]=>
  string(12) "Andy Sautins"
  [17]=>
  string(10) "Brian Wang"
  [26]=>
  string(4) "CURL"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [19]=>
  string(16) "Chris Vandomelen"
  [18]=>
  string(13) "Chad Robinson"
  [12]=>
  string(14) "Andrew Skalski"
  [11]=>
  string(13) "Andrew Avdeev"
  [4]=>
  string(17) "Alexander Aulbach"
  [3]=>
  string(13) "Alex Plotnick"
  [2]=>
  string(11) "Alex Belits"
  [1]=>
  string(11) "Alex Barkov"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [10]=>
  string(15) "Andrei Zmievski"
  [9]=>
  string(18) "Andreas Karajannis"
  [8]=>
  string(15) "Andre Langhorst"
  [7]=>
  string(12) "Andi Gutmans"
  [22]=>
  string(14) "Colin Viebrock"
  [27]=>
  string(9) "CyberCash"
  [42]=>
  string(12) "Evan Klinger"
  [41]=>
  string(11) "Eric Warnke"
  [40]=>
  string(11) "Egon Schmid"
  [39]=>
  string(12) "Doug DeJulio"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [46]=>
  string(11) "Harald Radi"
  [45]=>
  string(14) "Gerrit Thomson"
  [38]=>
  string(14) "Derick Rethans"
  [37]=>
  string(11) "David Sklar"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [28]=>
  string(18) "Daniel Beulshausen"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [36]=>
  string(11) "David Hénot"
  [35]=>
  string(12) "David Hedbor"
  [34]=>
  string(14) "David Eriksson"
  [0]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [26]=>
  string(4) "CURL"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
  ["CURL"]=>
  int(26)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
  ["CURL"]=>
  int(26)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
  ["CURL"]=>
  int(26)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Alex Belits"]=>
  int(2)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
  ["CURL"]=>
  int(26)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Alex Plotnick"]=>
  int(3)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Alexander Aulbach"]=>
  int(4)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Alexander Feldman"]=>
  int(5)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["Amitay Isaacs"]=>
  int(6)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Andi Gutmans"]=>
  int(7)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Andre Langhorst"]=>
  int(8)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Andreas Karajannis"]=>
  int(9)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["9.2233720368547E+18"]=>
  int(63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
}
array(6) {
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["9.2233720368547E+18"]=>
  int(63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
}
array(6) {
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["9.2233720368547E+18"]=>
  int(63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
}
array(6) {
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["9.2233720368547E+18"]=>
  int(63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
}
array(5) {
  [-2147483648]=>
  float(-2147483648)
  [-2147483647]=>
  int(-2147483647)
  [-2147483646]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(5) {
  [-2147483648]=>
  float(-2147483648)
  [-2147483647]=>
  int(-2147483647)
  [-2147483646]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(5) {
  [-2147483648]=>
  float(-2147483648)
  [-2147483647]=>
  int(-2147483647)
  [-2147483646]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(5) {
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(6) {
  ["-2147483648"]=>
  float(-2147483648)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483646"]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(6) {
  ["-2147483648"]=>
  float(-2147483648)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483646"]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(6) {
  ["-2147483648"]=>
  float(-2147483648)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483646"]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(6) {
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing krsort() -- 
array(93) {
  [92]=>
  string(12) "Zeev Suraski"
  [91]=>
  string(10) "Zak Greant"
  [90]=>
  string(11) "Wez Furlong"
  [89]=>
  string(11) "Vlad Krupin"
  [88]=>
  string(13) "Uwe Steinmann"
  [87]=>
  string(14) "Tsukada Takuya"
  [86]=>
  string(7) "Tom May"
  [85]=>
  string(16) "Thies C. Arntzen"
  [84]=>
  string(11) "Stig Venaas"
  [83]=>
  string(11) "Stig Bakken"
  [82]=>
  string(15) "Sterling Hughes"
  [81]=>
  string(16) "Stephanie Wehner"
  [80]=>
  string(15) "Stefan Roehrich"
  [79]=>
  string(14) "Slava Poliakov"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [76]=>
  string(18) "Sebastian Bergmann"
  [75]=>
  string(15) "Sascha Schumann"
  [74]=>
  string(14) "Sascha Kettler"
  [73]=>
  string(8) "Sam Ruby"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [70]=>
  string(16) "Ramil Kalimullin"
  [69]=>
  string(13) "Phil Driscoll"
  [68]=>
  string(15) "Olivier Cahagne"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [64]=>
  string(18) "Lars Torben Wilson"
  [63]=>
  string(17) "Kristian Köhntopp"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [61]=>
  string(10) "Jouni Ahto"
  [60]=>
  string(13) "John Donagher"
  [59]=>
  string(10) "Joey Smith"
  [58]=>
  string(12) "Jim Winstead"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [56]=>
  string(12) "Jason Greene"
  [55]=>
  string(13) "Jani Taskinen"
  [54]=>
  string(11) "Jan Borsodi"
  [53]=>
  string(11) "James Moore"
  [52]=>
  string(13) "Jalal Pushman"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [50]=>
  string(17) "Holger Zimmermann"
  [49]=>
  string(12) "Hojtsy Gabor"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [46]=>
  string(11) "Harald Radi"
  [45]=>
  string(14) "Gerrit Thomson"
  [44]=>
  string(12) "Fredrik Ohrn"
  [43]=>
  string(16) "Frank M. Kromann"
  [42]=>
  string(12) "Evan Klinger"
  [41]=>
  string(11) "Eric Warnke"
  [40]=>
  string(11) "Egon Schmid"
  [39]=>
  string(12) "Doug DeJulio"
  [38]=>
  string(14) "Derick Rethans"
  [37]=>
  string(11) "David Sklar"
  [36]=>
  string(11) "David Hénot"
  [35]=>
  string(12) "David Hedbor"
  [34]=>
  string(14) "David Eriksson"
  [33]=>
  string(11) "David Croft"
  [32]=>
  string(12) "David Benson"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [28]=>
  string(18) "Daniel Beulshausen"
  [27]=>
  string(9) "CyberCash"
  [26]=>
  string(4) "CURL"
  [25]=>
  string(5) "ctype"
  [24]=>
  string(5) "crack"
  [23]=>
  string(4) "cpdf"
  [22]=>
  string(14) "Colin Viebrock"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [19]=>
  string(16) "Chris Vandomelen"
  [18]=>
  string(13) "Chad Robinson"
  [17]=>
  string(10) "Brian Wang"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [13]=>
  string(12) "Andy Sautins"
  [12]=>
  string(14) "Andrew Skalski"
  [11]=>
  string(13) "Andrew Avdeev"
  [10]=>
  string(15) "Andrei Zmievski"
  [9]=>
  string(18) "Andreas Karajannis"
  [8]=>
  string(15) "Andre Langhorst"
  [7]=>
  string(12) "Andi Gutmans"
  [6]=>
  string(13) "Amitay Isaacs"
  [5]=>
  string(17) "Alexander Feldman"
  [4]=>
  string(17) "Alexander Aulbach"
  [3]=>
  string(13) "Alex Plotnick"
  [2]=>
  string(11) "Alex Belits"
  [1]=>
  string(11) "Alex Barkov"
  [0]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [92]=>
  string(12) "Zeev Suraski"
  [91]=>
  string(10) "Zak Greant"
  [90]=>
  string(11) "Wez Furlong"
  [89]=>
  string(11) "Vlad Krupin"
  [88]=>
  string(13) "Uwe Steinmann"
  [87]=>
  string(14) "Tsukada Takuya"
  [86]=>
  string(7) "Tom May"
  [85]=>
  string(16) "Thies C. Arntzen"
  [84]=>
  string(11) "Stig Venaas"
  [83]=>
  string(11) "Stig Bakken"
  [82]=>
  string(15) "Sterling Hughes"
  [81]=>
  string(16) "Stephanie Wehner"
  [80]=>
  string(15) "Stefan Roehrich"
  [79]=>
  string(14) "Slava Poliakov"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [76]=>
  string(18) "Sebastian Bergmann"
  [75]=>
  string(15) "Sascha Schumann"
  [74]=>
  string(14) "Sascha Kettler"
  [73]=>
  string(8) "Sam Ruby"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [70]=>
  string(16) "Ramil Kalimullin"
  [69]=>
  string(13) "Phil Driscoll"
  [68]=>
  string(15) "Olivier Cahagne"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [64]=>
  string(18) "Lars Torben Wilson"
  [63]=>
  string(17) "Kristian Köhntopp"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [61]=>
  string(10) "Jouni Ahto"
  [60]=>
  string(13) "John Donagher"
  [59]=>
  string(10) "Joey Smith"
  [58]=>
  string(12) "Jim Winstead"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [56]=>
  string(12) "Jason Greene"
  [55]=>
  string(13) "Jani Taskinen"
  [54]=>
  string(11) "Jan Borsodi"
  [53]=>
  string(11) "James Moore"
  [52]=>
  string(13) "Jalal Pushman"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [50]=>
  string(17) "Holger Zimmermann"
  [49]=>
  string(12) "Hojtsy Gabor"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [46]=>
  string(11) "Harald Radi"
  [45]=>
  string(14) "Gerrit Thomson"
  [44]=>
  string(12) "Fredrik Ohrn"
  [43]=>
  string(16) "Frank M. Kromann"
  [42]=>
  string(12) "Evan Klinger"
  [41]=>
  string(11) "Eric Warnke"
  [40]=>
  string(11) "Egon Schmid"
  [39]=>
  string(12) "Doug DeJulio"
  [38]=>
  string(14) "Derick Rethans"
  [37]=>
  string(11) "David Sklar"
  [36]=>
  string(11) "David Hénot"
  [35]=>
  string(12) "David Hedbor"
  [34]=>
  string(14) "David Eriksson"
  [33]=>
  string(11) "David Croft"
  [32]=>
  string(12) "David Benson"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [28]=>
  string(18) "Daniel Beulshausen"
  [27]=>
  string(9) "CyberCash"
  [26]=>
  string(4) "CURL"
  [25]=>
  string(5) "ctype"
  [24]=>
  string(5) "crack"
  [23]=>
  string(4) "cpdf"
  [22]=>
  string(14) "Colin Viebrock"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [19]=>
  string(16) "Chris Vandomelen"
  [18]=>
  string(13) "Chad Robinson"
  [17]=>
  string(10) "Brian Wang"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [13]=>
  string(12) "Andy Sautins"
  [12]=>
  string(14) "Andrew Skalski"
  [11]=>
  string(13) "Andrew Avdeev"
  [10]=>
  string(15) "Andrei Zmievski"
  [9]=>
  string(18) "Andreas Karajannis"
  [8]=>
  string(15) "Andre Langhorst"
  [7]=>
  string(12) "Andi Gutmans"
  [6]=>
  string(13) "Amitay Isaacs"
  [5]=>
  string(17) "Alexander Feldman"
  [4]=>
  string(17) "Alexander Aulbach"
  [3]=>
  string(13) "Alex Plotnick"
  [2]=>
  string(11) "Alex Belits"
  [1]=>
  string(11) "Alex Barkov"
  [0]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [92]=>
  string(12) "Zeev Suraski"
  [91]=>
  string(10) "Zak Greant"
  [90]=>
  string(11) "Wez Furlong"
  [89]=>
  string(11) "Vlad Krupin"
  [88]=>
  string(13) "Uwe Steinmann"
  [87]=>
  string(14) "Tsukada Takuya"
  [86]=>
  string(7) "Tom May"
  [85]=>
  string(16) "Thies C. Arntzen"
  [84]=>
  string(11) "Stig Venaas"
  [83]=>
  string(11) "Stig Bakken"
  [82]=>
  string(15) "Sterling Hughes"
  [81]=>
  string(16) "Stephanie Wehner"
  [80]=>
  string(15) "Stefan Roehrich"
  [79]=>
  string(14) "Slava Poliakov"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [76]=>
  string(18) "Sebastian Bergmann"
  [75]=>
  string(15) "Sascha Schumann"
  [74]=>
  string(14) "Sascha Kettler"
  [73]=>
  string(8) "Sam Ruby"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [70]=>
  string(16) "Ramil Kalimullin"
  [69]=>
  string(13) "Phil Driscoll"
  [68]=>
  string(15) "Olivier Cahagne"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [64]=>
  string(18) "Lars Torben Wilson"
  [63]=>
  string(17) "Kristian Köhntopp"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [61]=>
  string(10) "Jouni Ahto"
  [60]=>
  string(13) "John Donagher"
  [59]=>
  string(10) "Joey Smith"
  [58]=>
  string(12) "Jim Winstead"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [56]=>
  string(12) "Jason Greene"
  [55]=>
  string(13) "Jani Taskinen"
  [54]=>
  string(11) "Jan Borsodi"
  [53]=>
  string(11) "James Moore"
  [52]=>
  string(13) "Jalal Pushman"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [50]=>
  string(17) "Holger Zimmermann"
  [49]=>
  string(12) "Hojtsy Gabor"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [46]=>
  string(11) "Harald Radi"
  [45]=>
  string(14) "Gerrit Thomson"
  [44]=>
  string(12) "Fredrik Ohrn"
  [43]=>
  string(16) "Frank M. Kromann"
  [42]=>
  string(12) "Evan Klinger"
  [41]=>
  string(11) "Eric Warnke"
  [40]=>
  string(11) "Egon Schmid"
  [39]=>
  string(12) "Doug DeJulio"
  [38]=>
  string(14) "Derick Rethans"
  [37]=>
  string(11) "David Sklar"
  [36]=>
  string(11) "David Hénot"
  [35]=>
  string(12) "David Hedbor"
  [34]=>
  string(14) "David Eriksson"
  [33]=>
  string(11) "David Croft"
  [32]=>
  string(12) "David Benson"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [28]=>
  string(18) "Daniel Beulshausen"
  [27]=>
  string(9) "CyberCash"
  [26]=>
  string(4) "CURL"
  [25]=>
  string(5) "ctype"
  [24]=>
  string(5) "crack"
  [23]=>
  string(4) "cpdf"
  [22]=>
  string(14) "Colin Viebrock"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [19]=>
  string(16) "Chris Vandomelen"
  [18]=>
  string(13) "Chad Robinson"
  [17]=>
  string(10) "Brian Wang"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [13]=>
  string(12) "Andy Sautins"
  [12]=>
  string(14) "Andrew Skalski"
  [11]=>
  string(13) "Andrew Avdeev"
  [10]=>
  string(15) "Andrei Zmievski"
  [9]=>
  string(18) "Andreas Karajannis"
  [8]=>
  string(15) "Andre Langhorst"
  [7]=>
  string(12) "Andi Gutmans"
  [6]=>
  string(13) "Amitay Isaacs"
  [5]=>
  string(17) "Alexander Feldman"
  [4]=>
  string(17) "Alexander Aulbach"
  [3]=>
  string(13) "Alex Plotnick"
  [2]=>
  string(11) "Alex Belits"
  [1]=>
  string(11) "Alex Barkov"
  [0]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [92]=>
  string(12) "Zeev Suraski"
  [91]=>
  string(10) "Zak Greant"
  [90]=>
  string(11) "Wez Furlong"
  [9]=>
  string(18) "Andreas Karajannis"
  [89]=>
  string(11) "Vlad Krupin"
  [88]=>
  string(13) "Uwe Steinmann"
  [87]=>
  string(14) "Tsukada Takuya"
  [86]=>
  string(7) "Tom May"
  [85]=>
  string(16) "Thies C. Arntzen"
  [84]=>
  string(11) "Stig Venaas"
  [83]=>
  string(11) "Stig Bakken"
  [82]=>
  string(15) "Sterling Hughes"
  [81]=>
  string(16) "Stephanie Wehner"
  [80]=>
  string(15) "Stefan Roehrich"
  [8]=>
  string(15) "Andre Langhorst"
  [79]=>
  string(14) "Slava Poliakov"
  [78]=>
  string(13) "Shane Caraveo"
  [77]=>
  string(17) "Sergey Kartashoff"
  [76]=>
  string(18) "Sebastian Bergmann"
  [75]=>
  string(15) "Sascha Schumann"
  [74]=>
  string(14) "Sascha Kettler"
  [73]=>
  string(8) "Sam Ruby"
  [72]=>
  string(9) "Rex Logan"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [70]=>
  string(16) "Ramil Kalimullin"
  [7]=>
  string(12) "Andi Gutmans"
  [69]=>
  string(13) "Phil Driscoll"
  [68]=>
  string(15) "Olivier Cahagne"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [66]=>
  string(12) "Mitch Golden"
  [65]=>
  string(11) "Mark Musone"
  [64]=>
  string(18) "Lars Torben Wilson"
  [63]=>
  string(17) "Kristian Köhntopp"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [61]=>
  string(10) "Jouni Ahto"
  [60]=>
  string(13) "John Donagher"
  [6]=>
  string(13) "Amitay Isaacs"
  [59]=>
  string(10) "Joey Smith"
  [58]=>
  string(12) "Jim Winstead"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [56]=>
  string(12) "Jason Greene"
  [55]=>
  string(13) "Jani Taskinen"
  [54]=>
  string(11) "Jan Borsodi"
  [53]=>
  string(11) "James Moore"
  [52]=>
  string(13) "Jalal Pushman"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [50]=>
  string(17) "Holger Zimmermann"
  [5]=>
  string(17) "Alexander Feldman"
  [49]=>
  string(12) "Hojtsy Gabor"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [46]=>
  string(11) "Harald Radi"
  [45]=>
  string(14) "Gerrit Thomson"
  [44]=>
  string(12) "Fredrik Ohrn"
  [43]=>
  string(16) "Frank M. Kromann"
  [42]=>
  string(12) "Evan Klinger"
  [41]=>
  string(11) "Eric Warnke"
  [40]=>
  string(11) "Egon Schmid"
  [4]=>
  string(17) "Alexander Aulbach"
  [39]=>
  string(12) "Doug DeJulio"
  [38]=>
  string(14) "Derick Rethans"
  [37]=>
  string(11) "David Sklar"
  [36]=>
  string(11) "David Hénot"
  [35]=>
  string(12) "David Hedbor"
  [34]=>
  string(14) "David Eriksson"
  [33]=>
  string(11) "David Croft"
  [32]=>
  string(12) "David Benson"
  [31]=>
  string(11) "Dave Hayden"
  [30]=>
  string(11) "Danny Heijl"
  [3]=>
  string(13) "Alex Plotnick"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [28]=>
  string(18) "Daniel Beulshausen"
  [27]=>
  string(9) "CyberCash"
  [26]=>
  string(4) "CURL"
  [25]=>
  string(5) "ctype"
  [24]=>
  string(5) "crack"
  [23]=>
  string(4) "cpdf"
  [22]=>
  string(14) "Colin Viebrock"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [20]=>
  string(16) "Christian Cartus"
  [2]=>
  string(11) "Alex Belits"
  [19]=>
  string(16) "Chris Vandomelen"
  [18]=>
  string(13) "Chad Robinson"
  [17]=>
  string(10) "Brian Wang"
  [16]=>
  string(18) "Brendan W. McAdams"
  [15]=>
  string(11) "Boian Bonev"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [13]=>
  string(12) "Andy Sautins"
  [12]=>
  string(14) "Andrew Skalski"
  [11]=>
  string(13) "Andrew Avdeev"
  [10]=>
  string(15) "Andrei Zmievski"
  [1]=>
  string(11) "Alex Barkov"
  [0]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  ["ctype"]=>
  int(25)
  ["crack"]=>
  int(24)
  ["cpdf"]=>
  int(23)
  ["Zeev Suraski"]=>
  int(92)
  ["Zak Greant"]=>
  int(91)
  ["Wez Furlong"]=>
  int(90)
  ["Vlad Krupin"]=>
  int(89)
  ["Uwe Steinmann"]=>
  int(88)
  ["Tsukada Takuya"]=>
  int(87)
  ["Tom May"]=>
  int(86)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Stig Venaas"]=>
  int(84)
  ["Stig Bakken"]=>
  int(83)
  ["Sterling Hughes"]=>
  int(82)
  ["Stephanie Wehner"]=>
  int(81)
  ["Stefan Roehrich"]=>
  int(80)
  ["Slava Poliakov"]=>
  int(79)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sascha Schumann"]=>
  int(75)
  ["Sascha Kettler"]=>
  int(74)
  ["Sam Ruby"]=>
  int(73)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Phil Driscoll"]=>
  int(69)
  ["Olivier Cahagne"]=>
  int(68)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Jouni Ahto"]=>
  int(61)
  ["John Donagher"]=>
  int(60)
  ["Joey Smith"]=>
  int(59)
  ["Jim Winstead"]=>
  int(58)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jason Greene"]=>
  int(56)
  ["Jani Taskinen"]=>
  int(55)
  ["Jan Borsodi"]=>
  int(54)
  ["James Moore"]=>
  int(53)
  ["Jalal Pushman"]=>
  int(52)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Holger Zimmermann"]=>
  int(50)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Harald Radi"]=>
  int(46)
  ["Gerrit Thomson"]=>
  int(45)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Frank M. Kromann"]=>
  int(43)
  ["Evan Klinger"]=>
  int(42)
  ["Eric Warnke"]=>
  int(41)
  ["Egon Schmid"]=>
  int(40)
  ["Doug DeJulio"]=>
  int(39)
  ["Derick Rethans"]=>
  int(38)
  ["David Sklar"]=>
  int(37)
  ["David Hénot"]=>
  int(36)
  ["David Hedbor"]=>
  int(35)
  ["David Eriksson"]=>
  int(34)
  ["David Croft"]=>
  int(33)
  ["David Benson"]=>
  int(32)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Daniel Beulshausen"]=>
  int(28)
  ["CyberCash"]=>
  int(27)
  ["Colin Viebrock"]=>
  int(22)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Chris Vandomelen"]=>
  int(19)
  ["Chad Robinson"]=>
  int(18)
  ["CURL"]=>
  int(26)
  ["Brian Wang"]=>
  int(17)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Andy Sautins"]=>
  int(13)
  ["Andrew Skalski"]=>
  int(12)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andre Langhorst"]=>
  int(8)
  ["Andi Gutmans"]=>
  int(7)
  ["Amitay Isaacs"]=>
  int(6)
  ["Alexander Feldman"]=>
  int(5)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alex Plotnick"]=>
  int(3)
  ["Alex Belits"]=>
  int(2)
  ["Alex Barkov"]=>
  int(1)
  ["Adam Dickmeiss"]=>
  int(0)
}
array(93) {
  ["ctype"]=>
  int(25)
  ["crack"]=>
  int(24)
  ["cpdf"]=>
  int(23)
  ["Zeev Suraski"]=>
  int(92)
  ["Zak Greant"]=>
  int(91)
  ["Wez Furlong"]=>
  int(90)
  ["Vlad Krupin"]=>
  int(89)
  ["Uwe Steinmann"]=>
  int(88)
  ["Tsukada Takuya"]=>
  int(87)
  ["Tom May"]=>
  int(86)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Stig Venaas"]=>
  int(84)
  ["Stig Bakken"]=>
  int(83)
  ["Sterling Hughes"]=>
  int(82)
  ["Stephanie Wehner"]=>
  int(81)
  ["Stefan Roehrich"]=>
  int(80)
  ["Slava Poliakov"]=>
  int(79)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sascha Schumann"]=>
  int(75)
  ["Sascha Kettler"]=>
  int(74)
  ["Sam Ruby"]=>
  int(73)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Phil Driscoll"]=>
  int(69)
  ["Olivier Cahagne"]=>
  int(68)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Jouni Ahto"]=>
  int(61)
  ["John Donagher"]=>
  int(60)
  ["Joey Smith"]=>
  int(59)
  ["Jim Winstead"]=>
  int(58)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jason Greene"]=>
  int(56)
  ["Jani Taskinen"]=>
  int(55)
  ["Jan Borsodi"]=>
  int(54)
  ["James Moore"]=>
  int(53)
  ["Jalal Pushman"]=>
  int(52)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Holger Zimmermann"]=>
  int(50)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Harald Radi"]=>
  int(46)
  ["Gerrit Thomson"]=>
  int(45)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Frank M. Kromann"]=>
  int(43)
  ["Evan Klinger"]=>
  int(42)
  ["Eric Warnke"]=>
  int(41)
  ["Egon Schmid"]=>
  int(40)
  ["Doug DeJulio"]=>
  int(39)
  ["Derick Rethans"]=>
  int(38)
  ["David Sklar"]=>
  int(37)
  ["David Hénot"]=>
  int(36)
  ["David Hedbor"]=>
  int(35)
  ["David Eriksson"]=>
  int(34)
  ["David Croft"]=>
  int(33)
  ["David Benson"]=>
  int(32)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Daniel Beulshausen"]=>
  int(28)
  ["CyberCash"]=>
  int(27)
  ["Colin Viebrock"]=>
  int(22)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Chris Vandomelen"]=>
  int(19)
  ["Chad Robinson"]=>
  int(18)
  ["CURL"]=>
  int(26)
  ["Brian Wang"]=>
  int(17)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Andy Sautins"]=>
  int(13)
  ["Andrew Skalski"]=>
  int(12)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andre Langhorst"]=>
  int(8)
  ["Andi Gutmans"]=>
  int(7)
  ["Amitay Isaacs"]=>
  int(6)
  ["Alexander Feldman"]=>
  int(5)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alex Plotnick"]=>
  int(3)
  ["Alex Belits"]=>
  int(2)
  ["Alex Barkov"]=>
  int(1)
  ["Adam Dickmeiss"]=>
  int(0)
}
array(93) {
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Colin Viebrock"]=>
  int(22)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Frank M. Kromann"]=>
  int(43)
  ["Evan Klinger"]=>
  int(42)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Alexander Feldman"]=>
  int(5)
  ["Alexander Aulbach"]=>
  int(4)
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Brian Wang"]=>
  int(17)
  ["CURL"]=>
  int(26)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["Wez Furlong"]=>
  int(90)
  ["Vlad Krupin"]=>
  int(89)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["Jani Taskinen"]=>
  int(55)
  ["Jan Borsodi"]=>
  int(54)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["ctype"]=>
  int(25)
}
array(93) {
  ["ctype"]=>
  int(25)
  ["crack"]=>
  int(24)
  ["cpdf"]=>
  int(23)
  ["Zeev Suraski"]=>
  int(92)
  ["Zak Greant"]=>
  int(91)
  ["Wez Furlong"]=>
  int(90)
  ["Vlad Krupin"]=>
  int(89)
  ["Uwe Steinmann"]=>
  int(88)
  ["Tsukada Takuya"]=>
  int(87)
  ["Tom May"]=>
  int(86)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Stig Venaas"]=>
  int(84)
  ["Stig Bakken"]=>
  int(83)
  ["Sterling Hughes"]=>
  int(82)
  ["Stephanie Wehner"]=>
  int(81)
  ["Stefan Roehrich"]=>
  int(80)
  ["Slava Poliakov"]=>
  int(79)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sascha Schumann"]=>
  int(75)
  ["Sascha Kettler"]=>
  int(74)
  ["Sam Ruby"]=>
  int(73)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Phil Driscoll"]=>
  int(69)
  ["Olivier Cahagne"]=>
  int(68)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Jouni Ahto"]=>
  int(61)
  ["John Donagher"]=>
  int(60)
  ["Joey Smith"]=>
  int(59)
  ["Jim Winstead"]=>
  int(58)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jason Greene"]=>
  int(56)
  ["Jani Taskinen"]=>
  int(55)
  ["Jan Borsodi"]=>
  int(54)
  ["James Moore"]=>
  int(53)
  ["Jalal Pushman"]=>
  int(52)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Holger Zimmermann"]=>
  int(50)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Harald Radi"]=>
  int(46)
  ["Gerrit Thomson"]=>
  int(45)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Frank M. Kromann"]=>
  int(43)
  ["Evan Klinger"]=>
  int(42)
  ["Eric Warnke"]=>
  int(41)
  ["Egon Schmid"]=>
  int(40)
  ["Doug DeJulio"]=>
  int(39)
  ["Derick Rethans"]=>
  int(38)
  ["David Sklar"]=>
  int(37)
  ["David Hénot"]=>
  int(36)
  ["David Hedbor"]=>
  int(35)
  ["David Eriksson"]=>
  int(34)
  ["David Croft"]=>
  int(33)
  ["David Benson"]=>
  int(32)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Daniel Beulshausen"]=>
  int(28)
  ["CyberCash"]=>
  int(27)
  ["Colin Viebrock"]=>
  int(22)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Chris Vandomelen"]=>
  int(19)
  ["Chad Robinson"]=>
  int(18)
  ["CURL"]=>
  int(26)
  ["Brian Wang"]=>
  int(17)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Andy Sautins"]=>
  int(13)
  ["Andrew Skalski"]=>
  int(12)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andre Langhorst"]=>
  int(8)
  ["Andi Gutmans"]=>
  int(7)
  ["Amitay Isaacs"]=>
  int(6)
  ["Alexander Feldman"]=>
  int(5)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alex Plotnick"]=>
  int(3)
  ["Alex Belits"]=>
  int(2)
  ["Alex Barkov"]=>
  int(1)
  ["Adam Dickmeiss"]=>
  int(0)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  ["9.2233720368547E+18"]=>
  int(63)
  ["3.6893488147419E+19"]=>
  int(65)
  ["1.844674407371E+19"]=>
  int(64)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-1.844674407371E+19"]=>
  int(-64)
}
array(6) {
  ["9.2233720368547E+18"]=>
  int(63)
  ["3.6893488147419E+19"]=>
  int(65)
  ["1.844674407371E+19"]=>
  int(64)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-1.844674407371E+19"]=>
  int(-64)
}
array(6) {
  ["3.6893488147419E+19"]=>
  int(65)
  ["1.844674407371E+19"]=>
  int(64)
  ["9.2233720368547E+18"]=>
  int(63)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
}
array(6) {
  ["9.2233720368547E+18"]=>
  int(63)
  ["3.6893488147419E+19"]=>
  int(65)
  ["1.844674407371E+19"]=>
  int(64)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-1.844674407371E+19"]=>
  int(-64)
}
array(5) {
  [2147483647]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
}
array(5) {
  [2147483647]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
}
array(5) {
  [2147483647]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
}
array(5) {
  [2147483647]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  [-2147483648]=>
  float(-2147483648)
  [-2147483647]=>
  int(-2147483647)
  [-2147483646]=>
  int(-2147483646)
}
array(6) {
  ["2147483648"]=>
  float(2147483648)
  ["2147483647"]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  ["-2147483648"]=>
  float(-2147483648)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483646"]=>
  int(-2147483646)
}
array(6) {
  ["2147483648"]=>
  float(2147483648)
  ["2147483647"]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  ["-2147483648"]=>
  float(-2147483648)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483646"]=>
  int(-2147483646)
}
array(6) {
  ["2147483648"]=>
  float(2147483648)
  ["2147483647"]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
}
array(6) {
  ["2147483648"]=>
  float(2147483648)
  ["2147483647"]=>
  int(2147483647)
  [2147483646]=>
  int(2147483646)
  ["-2147483648"]=>
  float(-2147483648)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483646"]=>
  int(-2147483646)
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing ksort() -- 
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
  [26]=>
  string(4) "CURL"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
}
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
  [26]=>
  string(4) "CURL"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
}
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
  [26]=>
  string(4) "CURL"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
}
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [2]=>
  string(11) "Alex Belits"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
  [26]=>
  string(4) "CURL"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [3]=>
  string(13) "Alex Plotnick"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [4]=>
  string(17) "Alexander Aulbach"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [5]=>
  string(17) "Alexander Feldman"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [6]=>
  string(13) "Amitay Isaacs"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [7]=>
  string(12) "Andi Gutmans"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [8]=>
  string(15) "Andre Langhorst"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [9]=>
  string(18) "Andreas Karajannis"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["CURL"]=>
  int(26)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["CURL"]=>
  int(26)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
}
array(93) {
  ["Mitch Golden"]=>
  int(66)
  ["Mark Musone"]=>
  int(65)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Rex Logan"]=>
  int(72)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Phil Driscoll"]=>
  int(69)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Jouni Ahto"]=>
  int(61)
  ["Jan Borsodi"]=>
  int(54)
  ["James Moore"]=>
  int(53)
  ["Jalal Pushman"]=>
  int(52)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["John Donagher"]=>
  int(60)
  ["Joey Smith"]=>
  int(59)
  ["Jim Winstead"]=>
  int(58)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Vlad Krupin"]=>
  int(89)
  ["Uwe Steinmann"]=>
  int(88)
  ["Tsukada Takuya"]=>
  int(87)
  ["Tom May"]=>
  int(86)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["ctype"]=>
  int(25)
  ["crack"]=>
  int(24)
  ["cpdf"]=>
  int(23)
  ["Zeev Suraski"]=>
  int(92)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Stig Venaas"]=>
  int(84)
  ["Shane Caraveo"]=>
  int(78)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sascha Schumann"]=>
  int(75)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stig Bakken"]=>
  int(83)
  ["Sterling Hughes"]=>
  int(82)
  ["Stephanie Wehner"]=>
  int(81)
  ["Holger Zimmermann"]=>
  int(50)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Boian Bonev"]=>
  int(15)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Andy Sautins"]=>
  int(13)
  ["Brian Wang"]=>
  int(17)
  ["CURL"]=>
  int(26)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Christian Cartus"]=>
  int(20)
  ["Chris Vandomelen"]=>
  int(19)
  ["Chad Robinson"]=>
  int(18)
  ["Andrew Skalski"]=>
  int(12)
  ["Andrew Avdeev"]=>
  int(11)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alex Plotnick"]=>
  int(3)
  ["Alex Belits"]=>
  int(2)
  ["Alex Barkov"]=>
  int(1)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andre Langhorst"]=>
  int(8)
  ["Andi Gutmans"]=>
  int(7)
  ["Colin Viebrock"]=>
  int(22)
  ["CyberCash"]=>
  int(27)
  ["Evan Klinger"]=>
  int(42)
  ["Eric Warnke"]=>
  int(41)
  ["Egon Schmid"]=>
  int(40)
  ["Doug DeJulio"]=>
  int(39)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Harald Radi"]=>
  int(46)
  ["Gerrit Thomson"]=>
  int(45)
  ["Derick Rethans"]=>
  int(38)
  ["David Sklar"]=>
  int(37)
  ["Dave Hayden"]=>
  int(31)
  ["Danny Heijl"]=>
  int(30)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Daniel Beulshausen"]=>
  int(28)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Hénot"]=>
  int(36)
  ["David Hedbor"]=>
  int(35)
  ["David Eriksson"]=>
  int(34)
  ["Adam Dickmeiss"]=>
  int(0)
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["CURL"]=>
  int(26)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
  ["9.2233720368547E+18"]=>
  int(63)
}
array(6) {
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
  ["9.2233720368547E+18"]=>
  int(63)
}
array(6) {
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["9.2233720368547E+18"]=>
  int(63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
}
array(6) {
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
  ["9.2233720368547E+18"]=>
  int(63)
}
array(5) {
  [-2147483648]=>
  float(-2147483648)
  [-2147483647]=>
  int(-2147483647)
  [-2147483646]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(5) {
  [-2147483648]=>
  float(-2147483648)
  [-2147483647]=>
  int(-2147483647)
  [-2147483646]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(5) {
  [-2147483648]=>
  float(-2147483648)
  [-2147483647]=>
  int(-2147483647)
  [-2147483646]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(5) {
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(6) {
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(6) {
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(6) {
  ["-2147483648"]=>
  float(-2147483648)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483646"]=>
  int(-2147483646)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(6) {
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing rsort() -- 
array(93) {
  [0]=>
  string(5) "ctype"
  [1]=>
  string(5) "crack"
  [2]=>
  string(4) "cpdf"
  [3]=>
  string(12) "Zeev Suraski"
  [4]=>
  string(10) "Zak Greant"
  [5]=>
  string(11) "Wez Furlong"
  [6]=>
  string(11) "Vlad Krupin"
  [7]=>
  string(13) "Uwe Steinmann"
  [8]=>
  string(14) "Tsukada Takuya"
  [9]=>
  string(7) "Tom May"
  [10]=>
  string(16) "Thies C. Arntzen"
  [11]=>
  string(11) "Stig Venaas"
  [12]=>
  string(11) "Stig Bakken"
  [13]=>
  string(15) "Sterling Hughes"
  [14]=>
  string(16) "Stephanie Wehner"
  [15]=>
  string(15) "Stefan Roehrich"
  [16]=>
  string(14) "Slava Poliakov"
  [17]=>
  string(13) "Shane Caraveo"
  [18]=>
  string(17) "Sergey Kartashoff"
  [19]=>
  string(18) "Sebastian Bergmann"
  [20]=>
  string(15) "Sascha Schumann"
  [21]=>
  string(14) "Sascha Kettler"
  [22]=>
  string(8) "Sam Ruby"
  [23]=>
  string(9) "Rex Logan"
  [24]=>
  string(14) "Rasmus Lerdorf"
  [25]=>
  string(16) "Ramil Kalimullin"
  [26]=>
  string(13) "Phil Driscoll"
  [27]=>
  string(15) "Olivier Cahagne"
  [28]=>
  string(21) "Nikos Mavroyanopoulos"
  [29]=>
  string(12) "Mitch Golden"
  [30]=>
  string(11) "Mark Musone"
  [31]=>
  string(18) "Lars Torben Wilson"
  [32]=>
  string(17) "Kristian Köhntopp"
  [33]=>
  string(16) "Kaj-Michael Lang"
  [34]=>
  string(10) "Jouni Ahto"
  [35]=>
  string(13) "John Donagher"
  [36]=>
  string(10) "Joey Smith"
  [37]=>
  string(12) "Jim Winstead"
  [38]=>
  string(25) "Jayakumar Muthukumarasamy"
  [39]=>
  string(12) "Jason Greene"
  [40]=>
  string(13) "Jani Taskinen"
  [41]=>
  string(11) "Jan Borsodi"
  [42]=>
  string(11) "James Moore"
  [43]=>
  string(13) "Jalal Pushman"
  [44]=>
  string(16) "Ilia Alshanetsky"
  [45]=>
  string(17) "Holger Zimmermann"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(16) "Hellekin O. Wolf"
  [48]=>
  string(18) "Hartmut Holzgraefe"
  [49]=>
  string(11) "Harald Radi"
  [50]=>
  string(14) "Gerrit Thomson"
  [51]=>
  string(12) "Fredrik Ohrn"
  [52]=>
  string(16) "Frank M. Kromann"
  [53]=>
  string(12) "Evan Klinger"
  [54]=>
  string(11) "Eric Warnke"
  [55]=>
  string(11) "Egon Schmid"
  [56]=>
  string(12) "Doug DeJulio"
  [57]=>
  string(14) "Derick Rethans"
  [58]=>
  string(11) "David Sklar"
  [59]=>
  string(11) "David Hénot"
  [60]=>
  string(12) "David Hedbor"
  [61]=>
  string(14) "David Eriksson"
  [62]=>
  string(11) "David Croft"
  [63]=>
  string(12) "David Benson"
  [64]=>
  string(11) "Dave Hayden"
  [65]=>
  string(11) "Danny Heijl"
  [66]=>
  string(17) "Daniel R Kalowsky"
  [67]=>
  string(18) "Daniel Beulshausen"
  [68]=>
  string(9) "CyberCash"
  [69]=>
  string(14) "Colin Viebrock"
  [70]=>
  string(15) "Chuck Hagenbuch"
  [71]=>
  string(16) "Christian Cartus"
  [72]=>
  string(16) "Chris Vandomelen"
  [73]=>
  string(13) "Chad Robinson"
  [74]=>
  string(4) "CURL"
  [75]=>
  string(10) "Brian Wang"
  [76]=>
  string(18) "Brendan W. McAdams"
  [77]=>
  string(11) "Boian Bonev"
  [78]=>
  string(19) "Antoni Pamies Olive"
  [79]=>
  string(12) "Andy Sautins"
  [80]=>
  string(14) "Andrew Skalski"
  [81]=>
  string(13) "Andrew Avdeev"
  [82]=>
  string(15) "Andrei Zmievski"
  [83]=>
  string(18) "Andreas Karajannis"
  [84]=>
  string(15) "Andre Langhorst"
  [85]=>
  string(12) "Andi Gutmans"
  [86]=>
  string(13) "Amitay Isaacs"
  [87]=>
  string(17) "Alexander Feldman"
  [88]=>
  string(17) "Alexander Aulbach"
  [89]=>
  string(13) "Alex Plotnick"
  [90]=>
  string(11) "Alex Belits"
  [91]=>
  string(11) "Alex Barkov"
  [92]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [0]=>
  string(5) "ctype"
  [1]=>
  string(5) "crack"
  [2]=>
  string(4) "cpdf"
  [3]=>
  string(12) "Zeev Suraski"
  [4]=>
  string(10) "Zak Greant"
  [5]=>
  string(11) "Wez Furlong"
  [6]=>
  string(11) "Vlad Krupin"
  [7]=>
  string(13) "Uwe Steinmann"
  [8]=>
  string(14) "Tsukada Takuya"
  [9]=>
  string(7) "Tom May"
  [10]=>
  string(16) "Thies C. Arntzen"
  [11]=>
  string(11) "Stig Venaas"
  [12]=>
  string(11) "Stig Bakken"
  [13]=>
  string(15) "Sterling Hughes"
  [14]=>
  string(16) "Stephanie Wehner"
  [15]=>
  string(15) "Stefan Roehrich"
  [16]=>
  string(14) "Slava Poliakov"
  [17]=>
  string(13) "Shane Caraveo"
  [18]=>
  string(17) "Sergey Kartashoff"
  [19]=>
  string(18) "Sebastian Bergmann"
  [20]=>
  string(15) "Sascha Schumann"
  [21]=>
  string(14) "Sascha Kettler"
  [22]=>
  string(8) "Sam Ruby"
  [23]=>
  string(9) "Rex Logan"
  [24]=>
  string(14) "Rasmus Lerdorf"
  [25]=>
  string(16) "Ramil Kalimullin"
  [26]=>
  string(13) "Phil Driscoll"
  [27]=>
  string(15) "Olivier Cahagne"
  [28]=>
  string(21) "Nikos Mavroyanopoulos"
  [29]=>
  string(12) "Mitch Golden"
  [30]=>
  string(11) "Mark Musone"
  [31]=>
  string(18) "Lars Torben Wilson"
  [32]=>
  string(17) "Kristian Köhntopp"
  [33]=>
  string(16) "Kaj-Michael Lang"
  [34]=>
  string(10) "Jouni Ahto"
  [35]=>
  string(13) "John Donagher"
  [36]=>
  string(10) "Joey Smith"
  [37]=>
  string(12) "Jim Winstead"
  [38]=>
  string(25) "Jayakumar Muthukumarasamy"
  [39]=>
  string(12) "Jason Greene"
  [40]=>
  string(13) "Jani Taskinen"
  [41]=>
  string(11) "Jan Borsodi"
  [42]=>
  string(11) "James Moore"
  [43]=>
  string(13) "Jalal Pushman"
  [44]=>
  string(16) "Ilia Alshanetsky"
  [45]=>
  string(17) "Holger Zimmermann"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(16) "Hellekin O. Wolf"
  [48]=>
  string(18) "Hartmut Holzgraefe"
  [49]=>
  string(11) "Harald Radi"
  [50]=>
  string(14) "Gerrit Thomson"
  [51]=>
  string(12) "Fredrik Ohrn"
  [52]=>
  string(16) "Frank M. Kromann"
  [53]=>
  string(12) "Evan Klinger"
  [54]=>
  string(11) "Eric Warnke"
  [55]=>
  string(11) "Egon Schmid"
  [56]=>
  string(12) "Doug DeJulio"
  [57]=>
  string(14) "Derick Rethans"
  [58]=>
  string(11) "David Sklar"
  [59]=>
  string(11) "David Hénot"
  [60]=>
  string(12) "David Hedbor"
  [61]=>
  string(14) "David Eriksson"
  [62]=>
  string(11) "David Croft"
  [63]=>
  string(12) "David Benson"
  [64]=>
  string(11) "Dave Hayden"
  [65]=>
  string(11) "Danny Heijl"
  [66]=>
  string(17) "Daniel R Kalowsky"
  [67]=>
  string(18) "Daniel Beulshausen"
  [68]=>
  string(9) "CyberCash"
  [69]=>
  string(14) "Colin Viebrock"
  [70]=>
  string(15) "Chuck Hagenbuch"
  [71]=>
  string(16) "Christian Cartus"
  [72]=>
  string(16) "Chris Vandomelen"
  [73]=>
  string(13) "Chad Robinson"
  [74]=>
  string(4) "CURL"
  [75]=>
  string(10) "Brian Wang"
  [76]=>
  string(18) "Brendan W. McAdams"
  [77]=>
  string(11) "Boian Bonev"
  [78]=>
  string(19) "Antoni Pamies Olive"
  [79]=>
  string(12) "Andy Sautins"
  [80]=>
  string(14) "Andrew Skalski"
  [81]=>
  string(13) "Andrew Avdeev"
  [82]=>
  string(15) "Andrei Zmievski"
  [83]=>
  string(18) "Andreas Karajannis"
  [84]=>
  string(15) "Andre Langhorst"
  [85]=>
  string(12) "Andi Gutmans"
  [86]=>
  string(13) "Amitay Isaacs"
  [87]=>
  string(17) "Alexander Feldman"
  [88]=>
  string(17) "Alexander Aulbach"
  [89]=>
  string(13) "Alex Plotnick"
  [90]=>
  string(11) "Alex Belits"
  [91]=>
  string(11) "Alex Barkov"
  [92]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [0]=>
  string(12) "David Benson"
  [1]=>
  string(11) "David Croft"
  [2]=>
  string(14) "David Eriksson"
  [3]=>
  string(12) "David Hedbor"
  [4]=>
  string(11) "Dave Hayden"
  [5]=>
  string(11) "Danny Heijl"
  [6]=>
  string(14) "Colin Viebrock"
  [7]=>
  string(9) "CyberCash"
  [8]=>
  string(18) "Daniel Beulshausen"
  [9]=>
  string(17) "Daniel R Kalowsky"
  [10]=>
  string(11) "David Hénot"
  [11]=>
  string(11) "David Sklar"
  [12]=>
  string(12) "Fredrik Ohrn"
  [13]=>
  string(14) "Gerrit Thomson"
  [14]=>
  string(11) "Harald Radi"
  [15]=>
  string(18) "Hartmut Holzgraefe"
  [16]=>
  string(16) "Frank M. Kromann"
  [17]=>
  string(12) "Evan Klinger"
  [18]=>
  string(14) "Derick Rethans"
  [19]=>
  string(12) "Doug DeJulio"
  [20]=>
  string(11) "Egon Schmid"
  [21]=>
  string(11) "Eric Warnke"
  [22]=>
  string(15) "Chuck Hagenbuch"
  [23]=>
  string(16) "Christian Cartus"
  [24]=>
  string(13) "Amitay Isaacs"
  [25]=>
  string(12) "Andi Gutmans"
  [26]=>
  string(15) "Andre Langhorst"
  [27]=>
  string(18) "Andreas Karajannis"
  [28]=>
  string(17) "Alexander Feldman"
  [29]=>
  string(17) "Alexander Aulbach"
  [30]=>
  string(14) "Adam Dickmeiss"
  [31]=>
  string(11) "Alex Barkov"
  [32]=>
  string(11) "Alex Belits"
  [33]=>
  string(13) "Alex Plotnick"
  [34]=>
  string(15) "Andrei Zmievski"
  [35]=>
  string(13) "Andrew Avdeev"
  [36]=>
  string(10) "Brian Wang"
  [37]=>
  string(4) "CURL"
  [38]=>
  string(13) "Chad Robinson"
  [39]=>
  string(16) "Chris Vandomelen"
  [40]=>
  string(18) "Brendan W. McAdams"
  [41]=>
  string(11) "Boian Bonev"
  [42]=>
  string(14) "Andrew Skalski"
  [43]=>
  string(12) "Andy Sautins"
  [44]=>
  string(19) "Antoni Pamies Olive"
  [45]=>
  string(16) "Hellekin O. Wolf"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(14) "Slava Poliakov"
  [48]=>
  string(15) "Stefan Roehrich"
  [49]=>
  string(16) "Stephanie Wehner"
  [50]=>
  string(15) "Sterling Hughes"
  [51]=>
  string(13) "Shane Caraveo"
  [52]=>
  string(17) "Sergey Kartashoff"
  [53]=>
  string(8) "Sam Ruby"
  [54]=>
  string(14) "Sascha Kettler"
  [55]=>
  string(15) "Sascha Schumann"
  [56]=>
  string(18) "Sebastian Bergmann"
  [57]=>
  string(11) "Stig Bakken"
  [58]=>
  string(11) "Stig Venaas"
  [59]=>
  string(10) "Zak Greant"
  [60]=>
  string(12) "Zeev Suraski"
  [61]=>
  string(4) "cpdf"
  [62]=>
  string(5) "crack"
  [63]=>
  string(11) "Wez Furlong"
  [64]=>
  string(11) "Vlad Krupin"
  [65]=>
  string(16) "Thies C. Arntzen"
  [66]=>
  string(7) "Tom May"
  [67]=>
  string(14) "Tsukada Takuya"
  [68]=>
  string(13) "Uwe Steinmann"
  [69]=>
  string(9) "Rex Logan"
  [70]=>
  string(14) "Rasmus Lerdorf"
  [71]=>
  string(12) "Jason Greene"
  [72]=>
  string(25) "Jayakumar Muthukumarasamy"
  [73]=>
  string(12) "Jim Winstead"
  [74]=>
  string(10) "Joey Smith"
  [75]=>
  string(13) "Jani Taskinen"
  [76]=>
  string(11) "Jan Borsodi"
  [77]=>
  string(17) "Holger Zimmermann"
  [78]=>
  string(16) "Ilia Alshanetsky"
  [79]=>
  string(13) "Jalal Pushman"
  [80]=>
  string(11) "James Moore"
  [81]=>
  string(13) "John Donagher"
  [82]=>
  string(10) "Jouni Ahto"
  [83]=>
  string(21) "Nikos Mavroyanopoulos"
  [84]=>
  string(15) "Olivier Cahagne"
  [85]=>
  string(13) "Phil Driscoll"
  [86]=>
  string(16) "Ramil Kalimullin"
  [87]=>
  string(12) "Mitch Golden"
  [88]=>
  string(11) "Mark Musone"
  [89]=>
  string(16) "Kaj-Michael Lang"
  [90]=>
  string(17) "Kristian Köhntopp"
  [91]=>
  string(18) "Lars Torben Wilson"
  [92]=>
  string(5) "ctype"
}
array(93) {
  [0]=>
  string(5) "ctype"
  [1]=>
  string(5) "crack"
  [2]=>
  string(4) "cpdf"
  [3]=>
  string(12) "Zeev Suraski"
  [4]=>
  string(10) "Zak Greant"
  [5]=>
  string(11) "Wez Furlong"
  [6]=>
  string(11) "Vlad Krupin"
  [7]=>
  string(13) "Uwe Steinmann"
  [8]=>
  string(14) "Tsukada Takuya"
  [9]=>
  string(7) "Tom May"
  [10]=>
  string(16) "Thies C. Arntzen"
  [11]=>
  string(11) "Stig Venaas"
  [12]=>
  string(11) "Stig Bakken"
  [13]=>
  string(15) "Sterling Hughes"
  [14]=>
  string(16) "Stephanie Wehner"
  [15]=>
  string(15) "Stefan Roehrich"
  [16]=>
  string(14) "Slava Poliakov"
  [17]=>
  string(13) "Shane Caraveo"
  [18]=>
  string(17) "Sergey Kartashoff"
  [19]=>
  string(18) "Sebastian Bergmann"
  [20]=>
  string(15) "Sascha Schumann"
  [21]=>
  string(14) "Sascha Kettler"
  [22]=>
  string(8) "Sam Ruby"
  [23]=>
  string(9) "Rex Logan"
  [24]=>
  string(14) "Rasmus Lerdorf"
  [25]=>
  string(16) "Ramil Kalimullin"
  [26]=>
  string(13) "Phil Driscoll"
  [27]=>
  string(15) "Olivier Cahagne"
  [28]=>
  string(21) "Nikos Mavroyanopoulos"
  [29]=>
  string(12) "Mitch Golden"
  [30]=>
  string(11) "Mark Musone"
  [31]=>
  string(18) "Lars Torben Wilson"
  [32]=>
  string(17) "Kristian Köhntopp"
  [33]=>
  string(16) "Kaj-Michael Lang"
  [34]=>
  string(10) "Jouni Ahto"
  [35]=>
  string(13) "John Donagher"
  [36]=>
  string(10) "Joey Smith"
  [37]=>
  string(12) "Jim Winstead"
  [38]=>
  string(25) "Jayakumar Muthukumarasamy"
  [39]=>
  string(12) "Jason Greene"
  [40]=>
  string(13) "Jani Taskinen"
  [41]=>
  string(11) "Jan Borsodi"
  [42]=>
  string(11) "James Moore"
  [43]=>
  string(13) "Jalal Pushman"
  [44]=>
  string(16) "Ilia Alshanetsky"
  [45]=>
  string(17) "Holger Zimmermann"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(16) "Hellekin O. Wolf"
  [48]=>
  string(18) "Hartmut Holzgraefe"
  [49]=>
  string(11) "Harald Radi"
  [50]=>
  string(14) "Gerrit Thomson"
  [51]=>
  string(12) "Fredrik Ohrn"
  [52]=>
  string(16) "Frank M. Kromann"
  [53]=>
  string(12) "Evan Klinger"
  [54]=>
  string(11) "Eric Warnke"
  [55]=>
  string(11) "Egon Schmid"
  [56]=>
  string(12) "Doug DeJulio"
  [57]=>
  string(14) "Derick Rethans"
  [58]=>
  string(11) "David Sklar"
  [59]=>
  string(11) "David Hénot"
  [60]=>
  string(12) "David Hedbor"
  [61]=>
  string(14) "David Eriksson"
  [62]=>
  string(11) "David Croft"
  [63]=>
  string(12) "David Benson"
  [64]=>
  string(11) "Dave Hayden"
  [65]=>
  string(11) "Danny Heijl"
  [66]=>
  string(17) "Daniel R Kalowsky"
  [67]=>
  string(18) "Daniel Beulshausen"
  [68]=>
  string(9) "CyberCash"
  [69]=>
  string(14) "Colin Viebrock"
  [70]=>
  string(15) "Chuck Hagenbuch"
  [71]=>
  string(16) "Christian Cartus"
  [72]=>
  string(16) "Chris Vandomelen"
  [73]=>
  string(13) "Chad Robinson"
  [74]=>
  string(4) "CURL"
  [75]=>
  string(10) "Brian Wang"
  [76]=>
  string(18) "Brendan W. McAdams"
  [77]=>
  string(11) "Boian Bonev"
  [78]=>
  string(19) "Antoni Pamies Olive"
  [79]=>
  string(12) "Andy Sautins"
  [80]=>
  string(14) "Andrew Skalski"
  [81]=>
  string(13) "Andrew Avdeev"
  [82]=>
  string(15) "Andrei Zmievski"
  [83]=>
  string(18) "Andreas Karajannis"
  [84]=>
  string(15) "Andre Langhorst"
  [85]=>
  string(12) "Andi Gutmans"
  [86]=>
  string(13) "Amitay Isaacs"
  [87]=>
  string(17) "Alexander Feldman"
  [88]=>
  string(17) "Alexander Aulbach"
  [89]=>
  string(13) "Alex Plotnick"
  [90]=>
  string(11) "Alex Belits"
  [91]=>
  string(11) "Alex Barkov"
  [92]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [0]=>
  int(92)
  [1]=>
  int(91)
  [2]=>
  int(90)
  [3]=>
  int(89)
  [4]=>
  int(88)
  [5]=>
  int(87)
  [6]=>
  int(86)
  [7]=>
  int(85)
  [8]=>
  int(84)
  [9]=>
  int(83)
  [10]=>
  int(82)
  [11]=>
  int(81)
  [12]=>
  int(80)
  [13]=>
  int(79)
  [14]=>
  int(78)
  [15]=>
  int(77)
  [16]=>
  int(76)
  [17]=>
  int(75)
  [18]=>
  int(74)
  [19]=>
  int(73)
  [20]=>
  int(72)
  [21]=>
  int(71)
  [22]=>
  int(70)
  [23]=>
  int(69)
  [24]=>
  int(68)
  [25]=>
  int(67)
  [26]=>
  int(66)
  [27]=>
  int(65)
  [28]=>
  int(64)
  [29]=>
  int(63)
  [30]=>
  int(62)
  [31]=>
  int(61)
  [32]=>
  int(60)
  [33]=>
  int(59)
  [34]=>
  int(58)
  [35]=>
  int(57)
  [36]=>
  int(56)
  [37]=>
  int(55)
  [38]=>
  int(54)
  [39]=>
  int(53)
  [40]=>
  int(52)
  [41]=>
  int(51)
  [42]=>
  int(50)
  [43]=>
  int(49)
  [44]=>
  int(48)
  [45]=>
  int(47)
  [46]=>
  int(46)
  [47]=>
  int(45)
  [48]=>
  int(44)
  [49]=>
  int(43)
  [50]=>
  int(42)
  [51]=>
  int(41)
  [52]=>
  int(40)
  [53]=>
  int(39)
  [54]=>
  int(38)
  [55]=>
  int(37)
  [56]=>
  int(36)
  [57]=>
  int(35)
  [58]=>
  int(34)
  [59]=>
  int(33)
  [60]=>
  int(32)
  [61]=>
  int(31)
  [62]=>
  int(30)
  [63]=>
  int(29)
  [64]=>
  int(28)
  [65]=>
  int(27)
  [66]=>
  int(26)
  [67]=>
  int(25)
  [68]=>
  int(24)
  [69]=>
  int(23)
  [70]=>
  int(22)
  [71]=>
  int(21)
  [72]=>
  int(20)
  [73]=>
  int(19)
  [74]=>
  int(18)
  [75]=>
  int(17)
  [76]=>
  int(16)
  [77]=>
  int(15)
  [78]=>
  int(14)
  [79]=>
  int(13)
  [80]=>
  int(12)
  [81]=>
  int(11)
  [82]=>
  int(10)
  [83]=>
  int(9)
  [84]=>
  int(8)
  [85]=>
  int(7)
  [86]=>
  int(6)
  [87]=>
  int(5)
  [88]=>
  int(4)
  [89]=>
  int(3)
  [90]=>
  int(2)
  [91]=>
  int(1)
  [92]=>
  int(0)
}
array(93) {
  [0]=>
  int(92)
  [1]=>
  int(91)
  [2]=>
  int(90)
  [3]=>
  int(89)
  [4]=>
  int(88)
  [5]=>
  int(87)
  [6]=>
  int(86)
  [7]=>
  int(85)
  [8]=>
  int(84)
  [9]=>
  int(83)
  [10]=>
  int(82)
  [11]=>
  int(81)
  [12]=>
  int(80)
  [13]=>
  int(79)
  [14]=>
  int(78)
  [15]=>
  int(77)
  [16]=>
  int(76)
  [17]=>
  int(75)
  [18]=>
  int(74)
  [19]=>
  int(73)
  [20]=>
  int(72)
  [21]=>
  int(71)
  [22]=>
  int(70)
  [23]=>
  int(69)
  [24]=>
  int(68)
  [25]=>
  int(67)
  [26]=>
  int(66)
  [27]=>
  int(65)
  [28]=>
  int(64)
  [29]=>
  int(63)
  [30]=>
  int(62)
  [31]=>
  int(61)
  [32]=>
  int(60)
  [33]=>
  int(59)
  [34]=>
  int(58)
  [35]=>
  int(57)
  [36]=>
  int(56)
  [37]=>
  int(55)
  [38]=>
  int(54)
  [39]=>
  int(53)
  [40]=>
  int(52)
  [41]=>
  int(51)
  [42]=>
  int(50)
  [43]=>
  int(49)
  [44]=>
  int(48)
  [45]=>
  int(47)
  [46]=>
  int(46)
  [47]=>
  int(45)
  [48]=>
  int(44)
  [49]=>
  int(43)
  [50]=>
  int(42)
  [51]=>
  int(41)
  [52]=>
  int(40)
  [53]=>
  int(39)
  [54]=>
  int(38)
  [55]=>
  int(37)
  [56]=>
  int(36)
  [57]=>
  int(35)
  [58]=>
  int(34)
  [59]=>
  int(33)
  [60]=>
  int(32)
  [61]=>
  int(31)
  [62]=>
  int(30)
  [63]=>
  int(29)
  [64]=>
  int(28)
  [65]=>
  int(27)
  [66]=>
  int(26)
  [67]=>
  int(25)
  [68]=>
  int(24)
  [69]=>
  int(23)
  [70]=>
  int(22)
  [71]=>
  int(21)
  [72]=>
  int(20)
  [73]=>
  int(19)
  [74]=>
  int(18)
  [75]=>
  int(17)
  [76]=>
  int(16)
  [77]=>
  int(15)
  [78]=>
  int(14)
  [79]=>
  int(13)
  [80]=>
  int(12)
  [81]=>
  int(11)
  [82]=>
  int(10)
  [83]=>
  int(9)
  [84]=>
  int(8)
  [85]=>
  int(7)
  [86]=>
  int(6)
  [87]=>
  int(5)
  [88]=>
  int(4)
  [89]=>
  int(3)
  [90]=>
  int(2)
  [91]=>
  int(1)
  [92]=>
  int(0)
}
array(93) {
  [0]=>
  int(92)
  [1]=>
  int(91)
  [2]=>
  int(90)
  [3]=>
  int(89)
  [4]=>
  int(88)
  [5]=>
  int(87)
  [6]=>
  int(86)
  [7]=>
  int(85)
  [8]=>
  int(84)
  [9]=>
  int(83)
  [10]=>
  int(82)
  [11]=>
  int(81)
  [12]=>
  int(80)
  [13]=>
  int(79)
  [14]=>
  int(78)
  [15]=>
  int(77)
  [16]=>
  int(76)
  [17]=>
  int(75)
  [18]=>
  int(74)
  [19]=>
  int(73)
  [20]=>
  int(72)
  [21]=>
  int(71)
  [22]=>
  int(70)
  [23]=>
  int(69)
  [24]=>
  int(68)
  [25]=>
  int(67)
  [26]=>
  int(66)
  [27]=>
  int(65)
  [28]=>
  int(64)
  [29]=>
  int(63)
  [30]=>
  int(62)
  [31]=>
  int(61)
  [32]=>
  int(60)
  [33]=>
  int(59)
  [34]=>
  int(58)
  [35]=>
  int(57)
  [36]=>
  int(56)
  [37]=>
  int(55)
  [38]=>
  int(54)
  [39]=>
  int(53)
  [40]=>
  int(52)
  [41]=>
  int(51)
  [42]=>
  int(50)
  [43]=>
  int(49)
  [44]=>
  int(48)
  [45]=>
  int(47)
  [46]=>
  int(46)
  [47]=>
  int(45)
  [48]=>
  int(44)
  [49]=>
  int(43)
  [50]=>
  int(42)
  [51]=>
  int(41)
  [52]=>
  int(40)
  [53]=>
  int(39)
  [54]=>
  int(38)
  [55]=>
  int(37)
  [56]=>
  int(36)
  [57]=>
  int(35)
  [58]=>
  int(34)
  [59]=>
  int(33)
  [60]=>
  int(32)
  [61]=>
  int(31)
  [62]=>
  int(30)
  [63]=>
  int(29)
  [64]=>
  int(28)
  [65]=>
  int(27)
  [66]=>
  int(26)
  [67]=>
  int(25)
  [68]=>
  int(24)
  [69]=>
  int(23)
  [70]=>
  int(22)
  [71]=>
  int(21)
  [72]=>
  int(20)
  [73]=>
  int(19)
  [74]=>
  int(18)
  [75]=>
  int(17)
  [76]=>
  int(16)
  [77]=>
  int(15)
  [78]=>
  int(14)
  [79]=>
  int(13)
  [80]=>
  int(12)
  [81]=>
  int(11)
  [82]=>
  int(10)
  [83]=>
  int(9)
  [84]=>
  int(8)
  [85]=>
  int(7)
  [86]=>
  int(6)
  [87]=>
  int(5)
  [88]=>
  int(4)
  [89]=>
  int(3)
  [90]=>
  int(2)
  [91]=>
  int(1)
  [92]=>
  int(0)
}
array(93) {
  [0]=>
  int(92)
  [1]=>
  int(91)
  [2]=>
  int(90)
  [3]=>
  int(9)
  [4]=>
  int(89)
  [5]=>
  int(88)
  [6]=>
  int(87)
  [7]=>
  int(86)
  [8]=>
  int(85)
  [9]=>
  int(84)
  [10]=>
  int(83)
  [11]=>
  int(82)
  [12]=>
  int(81)
  [13]=>
  int(80)
  [14]=>
  int(8)
  [15]=>
  int(79)
  [16]=>
  int(78)
  [17]=>
  int(77)
  [18]=>
  int(76)
  [19]=>
  int(75)
  [20]=>
  int(74)
  [21]=>
  int(73)
  [22]=>
  int(72)
  [23]=>
  int(71)
  [24]=>
  int(70)
  [25]=>
  int(7)
  [26]=>
  int(69)
  [27]=>
  int(68)
  [28]=>
  int(67)
  [29]=>
  int(66)
  [30]=>
  int(65)
  [31]=>
  int(64)
  [32]=>
  int(63)
  [33]=>
  int(62)
  [34]=>
  int(61)
  [35]=>
  int(60)
  [36]=>
  int(6)
  [37]=>
  int(59)
  [38]=>
  int(58)
  [39]=>
  int(57)
  [40]=>
  int(56)
  [41]=>
  int(55)
  [42]=>
  int(54)
  [43]=>
  int(53)
  [44]=>
  int(52)
  [45]=>
  int(51)
  [46]=>
  int(50)
  [47]=>
  int(5)
  [48]=>
  int(49)
  [49]=>
  int(48)
  [50]=>
  int(47)
  [51]=>
  int(46)
  [52]=>
  int(45)
  [53]=>
  int(44)
  [54]=>
  int(43)
  [55]=>
  int(42)
  [56]=>
  int(41)
  [57]=>
  int(40)
  [58]=>
  int(4)
  [59]=>
  int(39)
  [60]=>
  int(38)
  [61]=>
  int(37)
  [62]=>
  int(36)
  [63]=>
  int(35)
  [64]=>
  int(34)
  [65]=>
  int(33)
  [66]=>
  int(32)
  [67]=>
  int(31)
  [68]=>
  int(30)
  [69]=>
  int(3)
  [70]=>
  int(29)
  [71]=>
  int(28)
  [72]=>
  int(27)
  [73]=>
  int(26)
  [74]=>
  int(25)
  [75]=>
  int(24)
  [76]=>
  int(23)
  [77]=>
  int(22)
  [78]=>
  int(21)
  [79]=>
  int(20)
  [80]=>
  int(2)
  [81]=>
  int(19)
  [82]=>
  int(18)
  [83]=>
  int(17)
  [84]=>
  int(16)
  [85]=>
  int(15)
  [86]=>
  int(14)
  [87]=>
  int(13)
  [88]=>
  int(12)
  [89]=>
  int(11)
  [90]=>
  int(10)
  [91]=>
  int(1)
  [92]=>
  int(0)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  [0]=>
  int(65)
  [1]=>
  int(64)
  [2]=>
  int(63)
  [3]=>
  int(-63)
  [4]=>
  int(-64)
  [5]=>
  int(-65)
}
array(6) {
  [0]=>
  int(65)
  [1]=>
  int(64)
  [2]=>
  int(63)
  [3]=>
  int(-63)
  [4]=>
  int(-64)
  [5]=>
  int(-65)
}
array(6) {
  [0]=>
  int(65)
  [1]=>
  int(64)
  [2]=>
  int(63)
  [3]=>
  int(-63)
  [4]=>
  int(-64)
  [5]=>
  int(-65)
}
array(6) {
  [0]=>
  int(65)
  [1]=>
  int(64)
  [2]=>
  int(63)
  [3]=>
  int(-65)
  [4]=>
  int(-64)
  [5]=>
  int(-63)
}
array(5) {
  [0]=>
  int(2147483647)
  [1]=>
  int(2147483646)
  [2]=>
  int(-2147483646)
  [3]=>
  int(-2147483647)
  [4]=>
  float(-2147483648)
}
array(5) {
  [0]=>
  int(2147483647)
  [1]=>
  int(2147483646)
  [2]=>
  int(-2147483646)
  [3]=>
  int(-2147483647)
  [4]=>
  float(-2147483648)
}
array(5) {
  [0]=>
  int(2147483647)
  [1]=>
  int(2147483646)
  [2]=>
  int(-2147483646)
  [3]=>
  int(-2147483647)
  [4]=>
  float(-2147483648)
}
array(5) {
  [0]=>
  int(2147483647)
  [1]=>
  int(2147483646)
  [2]=>
  float(-2147483648)
  [3]=>
  int(-2147483647)
  [4]=>
  int(-2147483646)
}
array(6) {
  [0]=>
  float(2147483648)
  [1]=>
  int(2147483647)
  [2]=>
  int(2147483646)
  [3]=>
  int(-2147483646)
  [4]=>
  int(-2147483647)
  [5]=>
  float(-2147483648)
}
array(6) {
  [0]=>
  float(2147483648)
  [1]=>
  int(2147483647)
  [2]=>
  int(2147483646)
  [3]=>
  int(-2147483646)
  [4]=>
  int(-2147483647)
  [5]=>
  float(-2147483648)
}
array(6) {
  [0]=>
  float(2147483648)
  [1]=>
  int(2147483647)
  [2]=>
  int(2147483646)
  [3]=>
  int(-2147483646)
  [4]=>
  int(-2147483647)
  [5]=>
  float(-2147483648)
}
array(6) {
  [0]=>
  float(2147483648)
  [1]=>
  int(2147483647)
  [2]=>
  int(2147483646)
  [3]=>
  float(-2147483648)
  [4]=>
  int(-2147483647)
  [5]=>
  int(-2147483646)
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing sort() -- 
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(4) "CURL"
  [19]=>
  string(13) "Chad Robinson"
  [20]=>
  string(16) "Chris Vandomelen"
  [21]=>
  string(16) "Christian Cartus"
  [22]=>
  string(15) "Chuck Hagenbuch"
  [23]=>
  string(14) "Colin Viebrock"
  [24]=>
  string(9) "CyberCash"
  [25]=>
  string(18) "Daniel Beulshausen"
  [26]=>
  string(17) "Daniel R Kalowsky"
  [27]=>
  string(11) "Danny Heijl"
  [28]=>
  string(11) "Dave Hayden"
  [29]=>
  string(12) "David Benson"
  [30]=>
  string(11) "David Croft"
  [31]=>
  string(14) "David Eriksson"
  [32]=>
  string(12) "David Hedbor"
  [33]=>
  string(11) "David Hénot"
  [34]=>
  string(11) "David Sklar"
  [35]=>
  string(14) "Derick Rethans"
  [36]=>
  string(12) "Doug DeJulio"
  [37]=>
  string(11) "Egon Schmid"
  [38]=>
  string(11) "Eric Warnke"
  [39]=>
  string(12) "Evan Klinger"
  [40]=>
  string(16) "Frank M. Kromann"
  [41]=>
  string(12) "Fredrik Ohrn"
  [42]=>
  string(14) "Gerrit Thomson"
  [43]=>
  string(11) "Harald Radi"
  [44]=>
  string(18) "Hartmut Holzgraefe"
  [45]=>
  string(16) "Hellekin O. Wolf"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(17) "Holger Zimmermann"
  [48]=>
  string(16) "Ilia Alshanetsky"
  [49]=>
  string(13) "Jalal Pushman"
  [50]=>
  string(11) "James Moore"
  [51]=>
  string(11) "Jan Borsodi"
  [52]=>
  string(13) "Jani Taskinen"
  [53]=>
  string(12) "Jason Greene"
  [54]=>
  string(25) "Jayakumar Muthukumarasamy"
  [55]=>
  string(12) "Jim Winstead"
  [56]=>
  string(10) "Joey Smith"
  [57]=>
  string(13) "John Donagher"
  [58]=>
  string(10) "Jouni Ahto"
  [59]=>
  string(16) "Kaj-Michael Lang"
  [60]=>
  string(17) "Kristian Köhntopp"
  [61]=>
  string(18) "Lars Torben Wilson"
  [62]=>
  string(11) "Mark Musone"
  [63]=>
  string(12) "Mitch Golden"
  [64]=>
  string(21) "Nikos Mavroyanopoulos"
  [65]=>
  string(15) "Olivier Cahagne"
  [66]=>
  string(13) "Phil Driscoll"
  [67]=>
  string(16) "Ramil Kalimullin"
  [68]=>
  string(14) "Rasmus Lerdorf"
  [69]=>
  string(9) "Rex Logan"
  [70]=>
  string(8) "Sam Ruby"
  [71]=>
  string(14) "Sascha Kettler"
  [72]=>
  string(15) "Sascha Schumann"
  [73]=>
  string(18) "Sebastian Bergmann"
  [74]=>
  string(17) "Sergey Kartashoff"
  [75]=>
  string(13) "Shane Caraveo"
  [76]=>
  string(14) "Slava Poliakov"
  [77]=>
  string(15) "Stefan Roehrich"
  [78]=>
  string(16) "Stephanie Wehner"
  [79]=>
  string(15) "Sterling Hughes"
  [80]=>
  string(11) "Stig Bakken"
  [81]=>
  string(11) "Stig Venaas"
  [82]=>
  string(16) "Thies C. Arntzen"
  [83]=>
  string(7) "Tom May"
  [84]=>
  string(14) "Tsukada Takuya"
  [85]=>
  string(13) "Uwe Steinmann"
  [86]=>
  string(11) "Vlad Krupin"
  [87]=>
  string(11) "Wez Furlong"
  [88]=>
  string(10) "Zak Greant"
  [89]=>
  string(12) "Zeev Suraski"
  [90]=>
  string(4) "cpdf"
  [91]=>
  string(5) "crack"
  [92]=>
  string(5) "ctype"
}
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(4) "CURL"
  [19]=>
  string(13) "Chad Robinson"
  [20]=>
  string(16) "Chris Vandomelen"
  [21]=>
  string(16) "Christian Cartus"
  [22]=>
  string(15) "Chuck Hagenbuch"
  [23]=>
  string(14) "Colin Viebrock"
  [24]=>
  string(9) "CyberCash"
  [25]=>
  string(18) "Daniel Beulshausen"
  [26]=>
  string(17) "Daniel R Kalowsky"
  [27]=>
  string(11) "Danny Heijl"
  [28]=>
  string(11) "Dave Hayden"
  [29]=>
  string(12) "David Benson"
  [30]=>
  string(11) "David Croft"
  [31]=>
  string(14) "David Eriksson"
  [32]=>
  string(12) "David Hedbor"
  [33]=>
  string(11) "David Hénot"
  [34]=>
  string(11) "David Sklar"
  [35]=>
  string(14) "Derick Rethans"
  [36]=>
  string(12) "Doug DeJulio"
  [37]=>
  string(11) "Egon Schmid"
  [38]=>
  string(11) "Eric Warnke"
  [39]=>
  string(12) "Evan Klinger"
  [40]=>
  string(16) "Frank M. Kromann"
  [41]=>
  string(12) "Fredrik Ohrn"
  [42]=>
  string(14) "Gerrit Thomson"
  [43]=>
  string(11) "Harald Radi"
  [44]=>
  string(18) "Hartmut Holzgraefe"
  [45]=>
  string(16) "Hellekin O. Wolf"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(17) "Holger Zimmermann"
  [48]=>
  string(16) "Ilia Alshanetsky"
  [49]=>
  string(13) "Jalal Pushman"
  [50]=>
  string(11) "James Moore"
  [51]=>
  string(11) "Jan Borsodi"
  [52]=>
  string(13) "Jani Taskinen"
  [53]=>
  string(12) "Jason Greene"
  [54]=>
  string(25) "Jayakumar Muthukumarasamy"
  [55]=>
  string(12) "Jim Winstead"
  [56]=>
  string(10) "Joey Smith"
  [57]=>
  string(13) "John Donagher"
  [58]=>
  string(10) "Jouni Ahto"
  [59]=>
  string(16) "Kaj-Michael Lang"
  [60]=>
  string(17) "Kristian Köhntopp"
  [61]=>
  string(18) "Lars Torben Wilson"
  [62]=>
  string(11) "Mark Musone"
  [63]=>
  string(12) "Mitch Golden"
  [64]=>
  string(21) "Nikos Mavroyanopoulos"
  [65]=>
  string(15) "Olivier Cahagne"
  [66]=>
  string(13) "Phil Driscoll"
  [67]=>
  string(16) "Ramil Kalimullin"
  [68]=>
  string(14) "Rasmus Lerdorf"
  [69]=>
  string(9) "Rex Logan"
  [70]=>
  string(8) "Sam Ruby"
  [71]=>
  string(14) "Sascha Kettler"
  [72]=>
  string(15) "Sascha Schumann"
  [73]=>
  string(18) "Sebastian Bergmann"
  [74]=>
  string(17) "Sergey Kartashoff"
  [75]=>
  string(13) "Shane Caraveo"
  [76]=>
  string(14) "Slava Poliakov"
  [77]=>
  string(15) "Stefan Roehrich"
  [78]=>
  string(16) "Stephanie Wehner"
  [79]=>
  string(15) "Sterling Hughes"
  [80]=>
  string(11) "Stig Bakken"
  [81]=>
  string(11) "Stig Venaas"
  [82]=>
  string(16) "Thies C. Arntzen"
  [83]=>
  string(7) "Tom May"
  [84]=>
  string(14) "Tsukada Takuya"
  [85]=>
  string(13) "Uwe Steinmann"
  [86]=>
  string(11) "Vlad Krupin"
  [87]=>
  string(11) "Wez Furlong"
  [88]=>
  string(10) "Zak Greant"
  [89]=>
  string(12) "Zeev Suraski"
  [90]=>
  string(4) "cpdf"
  [91]=>
  string(5) "crack"
  [92]=>
  string(5) "ctype"
}
array(93) {
  [0]=>
  string(12) "Mitch Golden"
  [1]=>
  string(11) "Mark Musone"
  [2]=>
  string(18) "Lars Torben Wilson"
  [3]=>
  string(17) "Kristian Köhntopp"
  [4]=>
  string(21) "Nikos Mavroyanopoulos"
  [5]=>
  string(15) "Olivier Cahagne"
  [6]=>
  string(9) "Rex Logan"
  [7]=>
  string(14) "Rasmus Lerdorf"
  [8]=>
  string(16) "Ramil Kalimullin"
  [9]=>
  string(13) "Phil Driscoll"
  [10]=>
  string(16) "Kaj-Michael Lang"
  [11]=>
  string(10) "Jouni Ahto"
  [12]=>
  string(11) "Jan Borsodi"
  [13]=>
  string(11) "James Moore"
  [14]=>
  string(13) "Jalal Pushman"
  [15]=>
  string(16) "Ilia Alshanetsky"
  [16]=>
  string(13) "Jani Taskinen"
  [17]=>
  string(12) "Jason Greene"
  [18]=>
  string(13) "John Donagher"
  [19]=>
  string(10) "Joey Smith"
  [20]=>
  string(12) "Jim Winstead"
  [21]=>
  string(25) "Jayakumar Muthukumarasamy"
  [22]=>
  string(8) "Sam Ruby"
  [23]=>
  string(14) "Sascha Kettler"
  [24]=>
  string(11) "Vlad Krupin"
  [25]=>
  string(13) "Uwe Steinmann"
  [26]=>
  string(14) "Tsukada Takuya"
  [27]=>
  string(7) "Tom May"
  [28]=>
  string(11) "Wez Furlong"
  [29]=>
  string(10) "Zak Greant"
  [30]=>
  string(5) "ctype"
  [31]=>
  string(5) "crack"
  [32]=>
  string(4) "cpdf"
  [33]=>
  string(12) "Zeev Suraski"
  [34]=>
  string(16) "Thies C. Arntzen"
  [35]=>
  string(11) "Stig Venaas"
  [36]=>
  string(13) "Shane Caraveo"
  [37]=>
  string(17) "Sergey Kartashoff"
  [38]=>
  string(18) "Sebastian Bergmann"
  [39]=>
  string(15) "Sascha Schumann"
  [40]=>
  string(14) "Slava Poliakov"
  [41]=>
  string(15) "Stefan Roehrich"
  [42]=>
  string(11) "Stig Bakken"
  [43]=>
  string(15) "Sterling Hughes"
  [44]=>
  string(16) "Stephanie Wehner"
  [45]=>
  string(17) "Holger Zimmermann"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(18) "Brendan W. McAdams"
  [48]=>
  string(11) "Boian Bonev"
  [49]=>
  string(19) "Antoni Pamies Olive"
  [50]=>
  string(12) "Andy Sautins"
  [51]=>
  string(10) "Brian Wang"
  [52]=>
  string(4) "CURL"
  [53]=>
  string(15) "Chuck Hagenbuch"
  [54]=>
  string(16) "Christian Cartus"
  [55]=>
  string(16) "Chris Vandomelen"
  [56]=>
  string(13) "Chad Robinson"
  [57]=>
  string(14) "Andrew Skalski"
  [58]=>
  string(13) "Andrew Avdeev"
  [59]=>
  string(17) "Alexander Aulbach"
  [60]=>
  string(13) "Alex Plotnick"
  [61]=>
  string(11) "Alex Belits"
  [62]=>
  string(11) "Alex Barkov"
  [63]=>
  string(17) "Alexander Feldman"
  [64]=>
  string(13) "Amitay Isaacs"
  [65]=>
  string(15) "Andrei Zmievski"
  [66]=>
  string(18) "Andreas Karajannis"
  [67]=>
  string(15) "Andre Langhorst"
  [68]=>
  string(12) "Andi Gutmans"
  [69]=>
  string(14) "Colin Viebrock"
  [70]=>
  string(9) "CyberCash"
  [71]=>
  string(12) "Evan Klinger"
  [72]=>
  string(11) "Eric Warnke"
  [73]=>
  string(11) "Egon Schmid"
  [74]=>
  string(12) "Doug DeJulio"
  [75]=>
  string(16) "Frank M. Kromann"
  [76]=>
  string(12) "Fredrik Ohrn"
  [77]=>
  string(16) "Hellekin O. Wolf"
  [78]=>
  string(18) "Hartmut Holzgraefe"
  [79]=>
  string(11) "Harald Radi"
  [80]=>
  string(14) "Gerrit Thomson"
  [81]=>
  string(14) "Derick Rethans"
  [82]=>
  string(11) "David Sklar"
  [83]=>
  string(11) "Dave Hayden"
  [84]=>
  string(11) "Danny Heijl"
  [85]=>
  string(17) "Daniel R Kalowsky"
  [86]=>
  string(18) "Daniel Beulshausen"
  [87]=>
  string(12) "David Benson"
  [88]=>
  string(11) "David Croft"
  [89]=>
  string(11) "David Hénot"
  [90]=>
  string(12) "David Hedbor"
  [91]=>
  string(14) "David Eriksson"
  [92]=>
  string(14) "Adam Dickmeiss"
}
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(4) "CURL"
  [19]=>
  string(13) "Chad Robinson"
  [20]=>
  string(16) "Chris Vandomelen"
  [21]=>
  string(16) "Christian Cartus"
  [22]=>
  string(15) "Chuck Hagenbuch"
  [23]=>
  string(14) "Colin Viebrock"
  [24]=>
  string(9) "CyberCash"
  [25]=>
  string(18) "Daniel Beulshausen"
  [26]=>
  string(17) "Daniel R Kalowsky"
  [27]=>
  string(11) "Danny Heijl"
  [28]=>
  string(11) "Dave Hayden"
  [29]=>
  string(12) "David Benson"
  [30]=>
  string(11) "David Croft"
  [31]=>
  string(14) "David Eriksson"
  [32]=>
  string(12) "David Hedbor"
  [33]=>
  string(11) "David Hénot"
  [34]=>
  string(11) "David Sklar"
  [35]=>
  string(14) "Derick Rethans"
  [36]=>
  string(12) "Doug DeJulio"
  [37]=>
  string(11) "Egon Schmid"
  [38]=>
  string(11) "Eric Warnke"
  [39]=>
  string(12) "Evan Klinger"
  [40]=>
  string(16) "Frank M. Kromann"
  [41]=>
  string(12) "Fredrik Ohrn"
  [42]=>
  string(14) "Gerrit Thomson"
  [43]=>
  string(11) "Harald Radi"
  [44]=>
  string(18) "Hartmut Holzgraefe"
  [45]=>
  string(16) "Hellekin O. Wolf"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(17) "Holger Zimmermann"
  [48]=>
  string(16) "Ilia Alshanetsky"
  [49]=>
  string(13) "Jalal Pushman"
  [50]=>
  string(11) "James Moore"
  [51]=>
  string(11) "Jan Borsodi"
  [52]=>
  string(13) "Jani Taskinen"
  [53]=>
  string(12) "Jason Greene"
  [54]=>
  string(25) "Jayakumar Muthukumarasamy"
  [55]=>
  string(12) "Jim Winstead"
  [56]=>
  string(10) "Joey Smith"
  [57]=>
  string(13) "John Donagher"
  [58]=>
  string(10) "Jouni Ahto"
  [59]=>
  string(16) "Kaj-Michael Lang"
  [60]=>
  string(17) "Kristian Köhntopp"
  [61]=>
  string(18) "Lars Torben Wilson"
  [62]=>
  string(11) "Mark Musone"
  [63]=>
  string(12) "Mitch Golden"
  [64]=>
  string(21) "Nikos Mavroyanopoulos"
  [65]=>
  string(15) "Olivier Cahagne"
  [66]=>
  string(13) "Phil Driscoll"
  [67]=>
  string(16) "Ramil Kalimullin"
  [68]=>
  string(14) "Rasmus Lerdorf"
  [69]=>
  string(9) "Rex Logan"
  [70]=>
  string(8) "Sam Ruby"
  [71]=>
  string(14) "Sascha Kettler"
  [72]=>
  string(15) "Sascha Schumann"
  [73]=>
  string(18) "Sebastian Bergmann"
  [74]=>
  string(17) "Sergey Kartashoff"
  [75]=>
  string(13) "Shane Caraveo"
  [76]=>
  string(14) "Slava Poliakov"
  [77]=>
  string(15) "Stefan Roehrich"
  [78]=>
  string(16) "Stephanie Wehner"
  [79]=>
  string(15) "Sterling Hughes"
  [80]=>
  string(11) "Stig Bakken"
  [81]=>
  string(11) "Stig Venaas"
  [82]=>
  string(16) "Thies C. Arntzen"
  [83]=>
  string(7) "Tom May"
  [84]=>
  string(14) "Tsukada Takuya"
  [85]=>
  string(13) "Uwe Steinmann"
  [86]=>
  string(11) "Vlad Krupin"
  [87]=>
  string(11) "Wez Furlong"
  [88]=>
  string(10) "Zak Greant"
  [89]=>
  string(12) "Zeev Suraski"
  [90]=>
  string(4) "cpdf"
  [91]=>
  string(5) "crack"
  [92]=>
  string(5) "ctype"
}
array(93) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
  [6]=>
  int(6)
  [7]=>
  int(7)
  [8]=>
  int(8)
  [9]=>
  int(9)
  [10]=>
  int(10)
  [11]=>
  int(11)
  [12]=>
  int(12)
  [13]=>
  int(13)
  [14]=>
  int(14)
  [15]=>
  int(15)
  [16]=>
  int(16)
  [17]=>
  int(17)
  [18]=>
  int(18)
  [19]=>
  int(19)
  [20]=>
  int(20)
  [21]=>
  int(21)
  [22]=>
  int(22)
  [23]=>
  int(23)
  [24]=>
  int(24)
  [25]=>
  int(25)
  [26]=>
  int(26)
  [27]=>
  int(27)
  [28]=>
  int(28)
  [29]=>
  int(29)
  [30]=>
  int(30)
  [31]=>
  int(31)
  [32]=>
  int(32)
  [33]=>
  int(33)
  [34]=>
  int(34)
  [35]=>
  int(35)
  [36]=>
  int(36)
  [37]=>
  int(37)
  [38]=>
  int(38)
  [39]=>
  int(39)
  [40]=>
  int(40)
  [41]=>
  int(41)
  [42]=>
  int(42)
  [43]=>
  int(43)
  [44]=>
  int(44)
  [45]=>
  int(45)
  [46]=>
  int(46)
  [47]=>
  int(47)
  [48]=>
  int(48)
  [49]=>
  int(49)
  [50]=>
  int(50)
  [51]=>
  int(51)
  [52]=>
  int(52)
  [53]=>
  int(53)
  [54]=>
  int(54)
  [55]=>
  int(55)
  [56]=>
  int(56)
  [57]=>
  int(57)
  [58]=>
  int(58)
  [59]=>
  int(59)
  [60]=>
  int(60)
  [61]=>
  int(61)
  [62]=>
  int(62)
  [63]=>
  int(63)
  [64]=>
  int(64)
  [65]=>
  int(65)
  [66]=>
  int(66)
  [67]=>
  int(67)
  [68]=>
  int(68)
  [69]=>
  int(69)
  [70]=>
  int(70)
  [71]=>
  int(71)
  [72]=>
  int(72)
  [73]=>
  int(73)
  [74]=>
  int(74)
  [75]=>
  int(75)
  [76]=>
  int(76)
  [77]=>
  int(77)
  [78]=>
  int(78)
  [79]=>
  int(79)
  [80]=>
  int(80)
  [81]=>
  int(81)
  [82]=>
  int(82)
  [83]=>
  int(83)
  [84]=>
  int(84)
  [85]=>
  int(85)
  [86]=>
  int(86)
  [87]=>
  int(87)
  [88]=>
  int(88)
  [89]=>
  int(89)
  [90]=>
  int(90)
  [91]=>
  int(91)
  [92]=>
  int(92)
}
array(93) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
  [6]=>
  int(6)
  [7]=>
  int(7)
  [8]=>
  int(8)
  [9]=>
  int(9)
  [10]=>
  int(10)
  [11]=>
  int(11)
  [12]=>
  int(12)
  [13]=>
  int(13)
  [14]=>
  int(14)
  [15]=>
  int(15)
  [16]=>
  int(16)
  [17]=>
  int(17)
  [18]=>
  int(18)
  [19]=>
  int(19)
  [20]=>
  int(20)
  [21]=>
  int(21)
  [22]=>
  int(22)
  [23]=>
  int(23)
  [24]=>
  int(24)
  [25]=>
  int(25)
  [26]=>
  int(26)
  [27]=>
  int(27)
  [28]=>
  int(28)
  [29]=>
  int(29)
  [30]=>
  int(30)
  [31]=>
  int(31)
  [32]=>
  int(32)
  [33]=>
  int(33)
  [34]=>
  int(34)
  [35]=>
  int(35)
  [36]=>
  int(36)
  [37]=>
  int(37)
  [38]=>
  int(38)
  [39]=>
  int(39)
  [40]=>
  int(40)
  [41]=>
  int(41)
  [42]=>
  int(42)
  [43]=>
  int(43)
  [44]=>
  int(44)
  [45]=>
  int(45)
  [46]=>
  int(46)
  [47]=>
  int(47)
  [48]=>
  int(48)
  [49]=>
  int(49)
  [50]=>
  int(50)
  [51]=>
  int(51)
  [52]=>
  int(52)
  [53]=>
  int(53)
  [54]=>
  int(54)
  [55]=>
  int(55)
  [56]=>
  int(56)
  [57]=>
  int(57)
  [58]=>
  int(58)
  [59]=>
  int(59)
  [60]=>
  int(60)
  [61]=>
  int(61)
  [62]=>
  int(62)
  [63]=>
  int(63)
  [64]=>
  int(64)
  [65]=>
  int(65)
  [66]=>
  int(66)
  [67]=>
  int(67)
  [68]=>
  int(68)
  [69]=>
  int(69)
  [70]=>
  int(70)
  [71]=>
  int(71)
  [72]=>
  int(72)
  [73]=>
  int(73)
  [74]=>
  int(74)
  [75]=>
  int(75)
  [76]=>
  int(76)
  [77]=>
  int(77)
  [78]=>
  int(78)
  [79]=>
  int(79)
  [80]=>
  int(80)
  [81]=>
  int(81)
  [82]=>
  int(82)
  [83]=>
  int(83)
  [84]=>
  int(84)
  [85]=>
  int(85)
  [86]=>
  int(86)
  [87]=>
  int(87)
  [88]=>
  int(88)
  [89]=>
  int(89)
  [90]=>
  int(90)
  [91]=>
  int(91)
  [92]=>
  int(92)
}
array(93) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
  [6]=>
  int(6)
  [7]=>
  int(7)
  [8]=>
  int(8)
  [9]=>
  int(9)
  [10]=>
  int(10)
  [11]=>
  int(11)
  [12]=>
  int(12)
  [13]=>
  int(13)
  [14]=>
  int(14)
  [15]=>
  int(15)
  [16]=>
  int(16)
  [17]=>
  int(17)
  [18]=>
  int(18)
  [19]=>
  int(19)
  [20]=>
  int(20)
  [21]=>
  int(21)
  [22]=>
  int(22)
  [23]=>
  int(23)
  [24]=>
  int(24)
  [25]=>
  int(25)
  [26]=>
  int(26)
  [27]=>
  int(27)
  [28]=>
  int(28)
  [29]=>
  int(29)
  [30]=>
  int(30)
  [31]=>
  int(31)
  [32]=>
  int(32)
  [33]=>
  int(33)
  [34]=>
  int(34)
  [35]=>
  int(35)
  [36]=>
  int(36)
  [37]=>
  int(37)
  [38]=>
  int(38)
  [39]=>
  int(39)
  [40]=>
  int(40)
  [41]=>
  int(41)
  [42]=>
  int(42)
  [43]=>
  int(43)
  [44]=>
  int(44)
  [45]=>
  int(45)
  [46]=>
  int(46)
  [47]=>
  int(47)
  [48]=>
  int(48)
  [49]=>
  int(49)
  [50]=>
  int(50)
  [51]=>
  int(51)
  [52]=>
  int(52)
  [53]=>
  int(53)
  [54]=>
  int(54)
  [55]=>
  int(55)
  [56]=>
  int(56)
  [57]=>
  int(57)
  [58]=>
  int(58)
  [59]=>
  int(59)
  [60]=>
  int(60)
  [61]=>
  int(61)
  [62]=>
  int(62)
  [63]=>
  int(63)
  [64]=>
  int(64)
  [65]=>
  int(65)
  [66]=>
  int(66)
  [67]=>
  int(67)
  [68]=>
  int(68)
  [69]=>
  int(69)
  [70]=>
  int(70)
  [71]=>
  int(71)
  [72]=>
  int(72)
  [73]=>
  int(73)
  [74]=>
  int(74)
  [75]=>
  int(75)
  [76]=>
  int(76)
  [77]=>
  int(77)
  [78]=>
  int(78)
  [79]=>
  int(79)
  [80]=>
  int(80)
  [81]=>
  int(81)
  [82]=>
  int(82)
  [83]=>
  int(83)
  [84]=>
  int(84)
  [85]=>
  int(85)
  [86]=>
  int(86)
  [87]=>
  int(87)
  [88]=>
  int(88)
  [89]=>
  int(89)
  [90]=>
  int(90)
  [91]=>
  int(91)
  [92]=>
  int(92)
}
array(93) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(10)
  [3]=>
  int(11)
  [4]=>
  int(12)
  [5]=>
  int(13)
  [6]=>
  int(14)
  [7]=>
  int(15)
  [8]=>
  int(16)
  [9]=>
  int(17)
  [10]=>
  int(18)
  [11]=>
  int(19)
  [12]=>
  int(2)
  [13]=>
  int(20)
  [14]=>
  int(21)
  [15]=>
  int(22)
  [16]=>
  int(23)
  [17]=>
  int(24)
  [18]=>
  int(25)
  [19]=>
  int(26)
  [20]=>
  int(27)
  [21]=>
  int(28)
  [22]=>
  int(29)
  [23]=>
  int(3)
  [24]=>
  int(30)
  [25]=>
  int(31)
  [26]=>
  int(32)
  [27]=>
  int(33)
  [28]=>
  int(34)
  [29]=>
  int(35)
  [30]=>
  int(36)
  [31]=>
  int(37)
  [32]=>
  int(38)
  [33]=>
  int(39)
  [34]=>
  int(4)
  [35]=>
  int(40)
  [36]=>
  int(41)
  [37]=>
  int(42)
  [38]=>
  int(43)
  [39]=>
  int(44)
  [40]=>
  int(45)
  [41]=>
  int(46)
  [42]=>
  int(47)
  [43]=>
  int(48)
  [44]=>
  int(49)
  [45]=>
  int(5)
  [46]=>
  int(50)
  [47]=>
  int(51)
  [48]=>
  int(52)
  [49]=>
  int(53)
  [50]=>
  int(54)
  [51]=>
  int(55)
  [52]=>
  int(56)
  [53]=>
  int(57)
  [54]=>
  int(58)
  [55]=>
  int(59)
  [56]=>
  int(6)
  [57]=>
  int(60)
  [58]=>
  int(61)
  [59]=>
  int(62)
  [60]=>
  int(63)
  [61]=>
  int(64)
  [62]=>
  int(65)
  [63]=>
  int(66)
  [64]=>
  int(67)
  [65]=>
  int(68)
  [66]=>
  int(69)
  [67]=>
  int(7)
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
  int(8)
  [79]=>
  int(80)
  [80]=>
  int(81)
  [81]=>
  int(82)
  [82]=>
  int(83)
  [83]=>
  int(84)
  [84]=>
  int(85)
  [85]=>
  int(86)
  [86]=>
  int(87)
  [87]=>
  int(88)
  [88]=>
  int(89)
  [89]=>
  int(9)
  [90]=>
  int(90)
  [91]=>
  int(91)
  [92]=>
  int(92)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  [0]=>
  int(-65)
  [1]=>
  int(-64)
  [2]=>
  int(-63)
  [3]=>
  int(63)
  [4]=>
  int(64)
  [5]=>
  int(65)
}
array(6) {
  [0]=>
  int(-65)
  [1]=>
  int(-64)
  [2]=>
  int(-63)
  [3]=>
  int(63)
  [4]=>
  int(64)
  [5]=>
  int(65)
}
array(6) {
  [0]=>
  int(-65)
  [1]=>
  int(-64)
  [2]=>
  int(-63)
  [3]=>
  int(63)
  [4]=>
  int(64)
  [5]=>
  int(65)
}
array(6) {
  [0]=>
  int(-63)
  [1]=>
  int(-64)
  [2]=>
  int(-65)
  [3]=>
  int(63)
  [4]=>
  int(64)
  [5]=>
  int(65)
}
array(5) {
  [0]=>
  float(-2147483648)
  [1]=>
  int(-2147483647)
  [2]=>
  int(-2147483646)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
}
array(5) {
  [0]=>
  float(-2147483648)
  [1]=>
  int(-2147483647)
  [2]=>
  int(-2147483646)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
}
array(5) {
  [0]=>
  float(-2147483648)
  [1]=>
  int(-2147483647)
  [2]=>
  int(-2147483646)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
}
array(5) {
  [0]=>
  int(-2147483646)
  [1]=>
  int(-2147483647)
  [2]=>
  float(-2147483648)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
}
array(6) {
  [0]=>
  float(-2147483648)
  [1]=>
  int(-2147483647)
  [2]=>
  int(-2147483646)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
  [5]=>
  float(2147483648)
}
array(6) {
  [0]=>
  float(-2147483648)
  [1]=>
  int(-2147483647)
  [2]=>
  int(-2147483646)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
  [5]=>
  float(2147483648)
}
array(6) {
  [0]=>
  float(-2147483648)
  [1]=>
  int(-2147483647)
  [2]=>
  int(-2147483646)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
  [5]=>
  float(2147483648)
}
array(6) {
  [0]=>
  int(-2147483646)
  [1]=>
  int(-2147483647)
  [2]=>
  float(-2147483648)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
  [5]=>
  float(2147483648)
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing uasort() -- 
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [26]=>
  string(4) "CURL"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Alex Belits"]=>
  int(2)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
  ["CURL"]=>
  int(26)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Alex Plotnick"]=>
  int(3)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Alexander Aulbach"]=>
  int(4)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Alexander Feldman"]=>
  int(5)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["Amitay Isaacs"]=>
  int(6)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Andi Gutmans"]=>
  int(7)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Andre Langhorst"]=>
  int(8)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Andreas Karajannis"]=>
  int(9)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["9.2233720368547E+18"]=>
  int(63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
}
array(5) {
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(6) {
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(0) {
}
array(5) {
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing uksort() -- 
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(13) "Chad Robinson"
  [19]=>
  string(16) "Chris Vandomelen"
  [2]=>
  string(11) "Alex Belits"
  [20]=>
  string(16) "Christian Cartus"
  [21]=>
  string(15) "Chuck Hagenbuch"
  [22]=>
  string(14) "Colin Viebrock"
  [23]=>
  string(4) "cpdf"
  [24]=>
  string(5) "crack"
  [25]=>
  string(5) "ctype"
  [26]=>
  string(4) "CURL"
  [27]=>
  string(9) "CyberCash"
  [28]=>
  string(18) "Daniel Beulshausen"
  [29]=>
  string(17) "Daniel R Kalowsky"
  [3]=>
  string(13) "Alex Plotnick"
  [30]=>
  string(11) "Danny Heijl"
  [31]=>
  string(11) "Dave Hayden"
  [32]=>
  string(12) "David Benson"
  [33]=>
  string(11) "David Croft"
  [34]=>
  string(14) "David Eriksson"
  [35]=>
  string(12) "David Hedbor"
  [36]=>
  string(11) "David Hénot"
  [37]=>
  string(11) "David Sklar"
  [38]=>
  string(14) "Derick Rethans"
  [39]=>
  string(12) "Doug DeJulio"
  [4]=>
  string(17) "Alexander Aulbach"
  [40]=>
  string(11) "Egon Schmid"
  [41]=>
  string(11) "Eric Warnke"
  [42]=>
  string(12) "Evan Klinger"
  [43]=>
  string(16) "Frank M. Kromann"
  [44]=>
  string(12) "Fredrik Ohrn"
  [45]=>
  string(14) "Gerrit Thomson"
  [46]=>
  string(11) "Harald Radi"
  [47]=>
  string(18) "Hartmut Holzgraefe"
  [48]=>
  string(16) "Hellekin O. Wolf"
  [49]=>
  string(12) "Hojtsy Gabor"
  [5]=>
  string(17) "Alexander Feldman"
  [50]=>
  string(17) "Holger Zimmermann"
  [51]=>
  string(16) "Ilia Alshanetsky"
  [52]=>
  string(13) "Jalal Pushman"
  [53]=>
  string(11) "James Moore"
  [54]=>
  string(11) "Jan Borsodi"
  [55]=>
  string(13) "Jani Taskinen"
  [56]=>
  string(12) "Jason Greene"
  [57]=>
  string(25) "Jayakumar Muthukumarasamy"
  [58]=>
  string(12) "Jim Winstead"
  [59]=>
  string(10) "Joey Smith"
  [6]=>
  string(13) "Amitay Isaacs"
  [60]=>
  string(13) "John Donagher"
  [61]=>
  string(10) "Jouni Ahto"
  [62]=>
  string(16) "Kaj-Michael Lang"
  [63]=>
  string(17) "Kristian Köhntopp"
  [64]=>
  string(18) "Lars Torben Wilson"
  [65]=>
  string(11) "Mark Musone"
  [66]=>
  string(12) "Mitch Golden"
  [67]=>
  string(21) "Nikos Mavroyanopoulos"
  [68]=>
  string(15) "Olivier Cahagne"
  [69]=>
  string(13) "Phil Driscoll"
  [7]=>
  string(12) "Andi Gutmans"
  [70]=>
  string(16) "Ramil Kalimullin"
  [71]=>
  string(14) "Rasmus Lerdorf"
  [72]=>
  string(9) "Rex Logan"
  [73]=>
  string(8) "Sam Ruby"
  [74]=>
  string(14) "Sascha Kettler"
  [75]=>
  string(15) "Sascha Schumann"
  [76]=>
  string(18) "Sebastian Bergmann"
  [77]=>
  string(17) "Sergey Kartashoff"
  [78]=>
  string(13) "Shane Caraveo"
  [79]=>
  string(14) "Slava Poliakov"
  [8]=>
  string(15) "Andre Langhorst"
  [80]=>
  string(15) "Stefan Roehrich"
  [81]=>
  string(16) "Stephanie Wehner"
  [82]=>
  string(15) "Sterling Hughes"
  [83]=>
  string(11) "Stig Bakken"
  [84]=>
  string(11) "Stig Venaas"
  [85]=>
  string(16) "Thies C. Arntzen"
  [86]=>
  string(7) "Tom May"
  [87]=>
  string(14) "Tsukada Takuya"
  [88]=>
  string(13) "Uwe Steinmann"
  [89]=>
  string(11) "Vlad Krupin"
  [9]=>
  string(18) "Andreas Karajannis"
  [90]=>
  string(11) "Wez Furlong"
  [91]=>
  string(10) "Zak Greant"
  [92]=>
  string(12) "Zeev Suraski"
}
array(93) {
  ["Adam Dickmeiss"]=>
  int(0)
  ["Alex Barkov"]=>
  int(1)
  ["Alex Belits"]=>
  int(2)
  ["Alex Plotnick"]=>
  int(3)
  ["Alexander Aulbach"]=>
  int(4)
  ["Alexander Feldman"]=>
  int(5)
  ["Amitay Isaacs"]=>
  int(6)
  ["Andi Gutmans"]=>
  int(7)
  ["Andre Langhorst"]=>
  int(8)
  ["Andreas Karajannis"]=>
  int(9)
  ["Andrei Zmievski"]=>
  int(10)
  ["Andrew Avdeev"]=>
  int(11)
  ["Andrew Skalski"]=>
  int(12)
  ["Andy Sautins"]=>
  int(13)
  ["Antoni Pamies Olive"]=>
  int(14)
  ["Boian Bonev"]=>
  int(15)
  ["Brendan W. McAdams"]=>
  int(16)
  ["Brian Wang"]=>
  int(17)
  ["CURL"]=>
  int(26)
  ["Chad Robinson"]=>
  int(18)
  ["Chris Vandomelen"]=>
  int(19)
  ["Christian Cartus"]=>
  int(20)
  ["Chuck Hagenbuch"]=>
  int(21)
  ["Colin Viebrock"]=>
  int(22)
  ["CyberCash"]=>
  int(27)
  ["Daniel Beulshausen"]=>
  int(28)
  ["Daniel R Kalowsky"]=>
  int(29)
  ["Danny Heijl"]=>
  int(30)
  ["Dave Hayden"]=>
  int(31)
  ["David Benson"]=>
  int(32)
  ["David Croft"]=>
  int(33)
  ["David Eriksson"]=>
  int(34)
  ["David Hedbor"]=>
  int(35)
  ["David Hénot"]=>
  int(36)
  ["David Sklar"]=>
  int(37)
  ["Derick Rethans"]=>
  int(38)
  ["Doug DeJulio"]=>
  int(39)
  ["Egon Schmid"]=>
  int(40)
  ["Eric Warnke"]=>
  int(41)
  ["Evan Klinger"]=>
  int(42)
  ["Frank M. Kromann"]=>
  int(43)
  ["Fredrik Ohrn"]=>
  int(44)
  ["Gerrit Thomson"]=>
  int(45)
  ["Harald Radi"]=>
  int(46)
  ["Hartmut Holzgraefe"]=>
  int(47)
  ["Hellekin O. Wolf"]=>
  int(48)
  ["Hojtsy Gabor"]=>
  int(49)
  ["Holger Zimmermann"]=>
  int(50)
  ["Ilia Alshanetsky"]=>
  int(51)
  ["Jalal Pushman"]=>
  int(52)
  ["James Moore"]=>
  int(53)
  ["Jan Borsodi"]=>
  int(54)
  ["Jani Taskinen"]=>
  int(55)
  ["Jason Greene"]=>
  int(56)
  ["Jayakumar Muthukumarasamy"]=>
  int(57)
  ["Jim Winstead"]=>
  int(58)
  ["Joey Smith"]=>
  int(59)
  ["John Donagher"]=>
  int(60)
  ["Jouni Ahto"]=>
  int(61)
  ["Kaj-Michael Lang"]=>
  int(62)
  ["Kristian Köhntopp"]=>
  int(63)
  ["Lars Torben Wilson"]=>
  int(64)
  ["Mark Musone"]=>
  int(65)
  ["Mitch Golden"]=>
  int(66)
  ["Nikos Mavroyanopoulos"]=>
  int(67)
  ["Olivier Cahagne"]=>
  int(68)
  ["Phil Driscoll"]=>
  int(69)
  ["Ramil Kalimullin"]=>
  int(70)
  ["Rasmus Lerdorf"]=>
  int(71)
  ["Rex Logan"]=>
  int(72)
  ["Sam Ruby"]=>
  int(73)
  ["Sascha Kettler"]=>
  int(74)
  ["Sascha Schumann"]=>
  int(75)
  ["Sebastian Bergmann"]=>
  int(76)
  ["Sergey Kartashoff"]=>
  int(77)
  ["Shane Caraveo"]=>
  int(78)
  ["Slava Poliakov"]=>
  int(79)
  ["Stefan Roehrich"]=>
  int(80)
  ["Stephanie Wehner"]=>
  int(81)
  ["Sterling Hughes"]=>
  int(82)
  ["Stig Bakken"]=>
  int(83)
  ["Stig Venaas"]=>
  int(84)
  ["Thies C. Arntzen"]=>
  int(85)
  ["Tom May"]=>
  int(86)
  ["Tsukada Takuya"]=>
  int(87)
  ["Uwe Steinmann"]=>
  int(88)
  ["Vlad Krupin"]=>
  int(89)
  ["Wez Furlong"]=>
  int(90)
  ["Zak Greant"]=>
  int(91)
  ["Zeev Suraski"]=>
  int(92)
  ["cpdf"]=>
  int(23)
  ["crack"]=>
  int(24)
  ["ctype"]=>
  int(25)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  ["-1.844674407371E+19"]=>
  int(-64)
  ["-3.6893488147419E+19"]=>
  int(-65)
  ["-9.2233720368547E+18"]=>
  int(-63)
  ["1.844674407371E+19"]=>
  int(64)
  ["3.6893488147419E+19"]=>
  int(65)
  ["9.2233720368547E+18"]=>
  int(63)
}
array(5) {
  [-2147483646]=>
  int(-2147483646)
  [-2147483647]=>
  int(-2147483647)
  [-2147483648]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  [2147483647]=>
  int(2147483647)
}
array(6) {
  ["-2147483646"]=>
  int(-2147483646)
  ["-2147483647"]=>
  int(-2147483647)
  ["-2147483648"]=>
  float(-2147483648)
  [2147483646]=>
  int(2147483646)
  ["2147483647"]=>
  int(2147483647)
  ["2147483648"]=>
  float(2147483648)
}
array(0) {
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}

 -- Testing usort() -- 
array(93) {
  [0]=>
  string(14) "Adam Dickmeiss"
  [1]=>
  string(11) "Alex Barkov"
  [2]=>
  string(11) "Alex Belits"
  [3]=>
  string(13) "Alex Plotnick"
  [4]=>
  string(17) "Alexander Aulbach"
  [5]=>
  string(17) "Alexander Feldman"
  [6]=>
  string(13) "Amitay Isaacs"
  [7]=>
  string(12) "Andi Gutmans"
  [8]=>
  string(15) "Andre Langhorst"
  [9]=>
  string(18) "Andreas Karajannis"
  [10]=>
  string(15) "Andrei Zmievski"
  [11]=>
  string(13) "Andrew Avdeev"
  [12]=>
  string(14) "Andrew Skalski"
  [13]=>
  string(12) "Andy Sautins"
  [14]=>
  string(19) "Antoni Pamies Olive"
  [15]=>
  string(11) "Boian Bonev"
  [16]=>
  string(18) "Brendan W. McAdams"
  [17]=>
  string(10) "Brian Wang"
  [18]=>
  string(4) "CURL"
  [19]=>
  string(13) "Chad Robinson"
  [20]=>
  string(16) "Chris Vandomelen"
  [21]=>
  string(16) "Christian Cartus"
  [22]=>
  string(15) "Chuck Hagenbuch"
  [23]=>
  string(14) "Colin Viebrock"
  [24]=>
  string(9) "CyberCash"
  [25]=>
  string(18) "Daniel Beulshausen"
  [26]=>
  string(17) "Daniel R Kalowsky"
  [27]=>
  string(11) "Danny Heijl"
  [28]=>
  string(11) "Dave Hayden"
  [29]=>
  string(12) "David Benson"
  [30]=>
  string(11) "David Croft"
  [31]=>
  string(14) "David Eriksson"
  [32]=>
  string(12) "David Hedbor"
  [33]=>
  string(11) "David Hénot"
  [34]=>
  string(11) "David Sklar"
  [35]=>
  string(14) "Derick Rethans"
  [36]=>
  string(12) "Doug DeJulio"
  [37]=>
  string(11) "Egon Schmid"
  [38]=>
  string(11) "Eric Warnke"
  [39]=>
  string(12) "Evan Klinger"
  [40]=>
  string(16) "Frank M. Kromann"
  [41]=>
  string(12) "Fredrik Ohrn"
  [42]=>
  string(14) "Gerrit Thomson"
  [43]=>
  string(11) "Harald Radi"
  [44]=>
  string(18) "Hartmut Holzgraefe"
  [45]=>
  string(16) "Hellekin O. Wolf"
  [46]=>
  string(12) "Hojtsy Gabor"
  [47]=>
  string(17) "Holger Zimmermann"
  [48]=>
  string(16) "Ilia Alshanetsky"
  [49]=>
  string(13) "Jalal Pushman"
  [50]=>
  string(11) "James Moore"
  [51]=>
  string(11) "Jan Borsodi"
  [52]=>
  string(13) "Jani Taskinen"
  [53]=>
  string(12) "Jason Greene"
  [54]=>
  string(25) "Jayakumar Muthukumarasamy"
  [55]=>
  string(12) "Jim Winstead"
  [56]=>
  string(10) "Joey Smith"
  [57]=>
  string(13) "John Donagher"
  [58]=>
  string(10) "Jouni Ahto"
  [59]=>
  string(16) "Kaj-Michael Lang"
  [60]=>
  string(17) "Kristian Köhntopp"
  [61]=>
  string(18) "Lars Torben Wilson"
  [62]=>
  string(11) "Mark Musone"
  [63]=>
  string(12) "Mitch Golden"
  [64]=>
  string(21) "Nikos Mavroyanopoulos"
  [65]=>
  string(15) "Olivier Cahagne"
  [66]=>
  string(13) "Phil Driscoll"
  [67]=>
  string(16) "Ramil Kalimullin"
  [68]=>
  string(14) "Rasmus Lerdorf"
  [69]=>
  string(9) "Rex Logan"
  [70]=>
  string(8) "Sam Ruby"
  [71]=>
  string(14) "Sascha Kettler"
  [72]=>
  string(15) "Sascha Schumann"
  [73]=>
  string(18) "Sebastian Bergmann"
  [74]=>
  string(17) "Sergey Kartashoff"
  [75]=>
  string(13) "Shane Caraveo"
  [76]=>
  string(14) "Slava Poliakov"
  [77]=>
  string(15) "Stefan Roehrich"
  [78]=>
  string(16) "Stephanie Wehner"
  [79]=>
  string(15) "Sterling Hughes"
  [80]=>
  string(11) "Stig Bakken"
  [81]=>
  string(11) "Stig Venaas"
  [82]=>
  string(16) "Thies C. Arntzen"
  [83]=>
  string(7) "Tom May"
  [84]=>
  string(14) "Tsukada Takuya"
  [85]=>
  string(13) "Uwe Steinmann"
  [86]=>
  string(11) "Vlad Krupin"
  [87]=>
  string(11) "Wez Furlong"
  [88]=>
  string(10) "Zak Greant"
  [89]=>
  string(12) "Zeev Suraski"
  [90]=>
  string(4) "cpdf"
  [91]=>
  string(5) "crack"
  [92]=>
  string(5) "ctype"
}
array(93) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(10)
  [3]=>
  int(11)
  [4]=>
  int(12)
  [5]=>
  int(13)
  [6]=>
  int(14)
  [7]=>
  int(15)
  [8]=>
  int(16)
  [9]=>
  int(17)
  [10]=>
  int(18)
  [11]=>
  int(19)
  [12]=>
  int(2)
  [13]=>
  int(20)
  [14]=>
  int(21)
  [15]=>
  int(22)
  [16]=>
  int(23)
  [17]=>
  int(24)
  [18]=>
  int(25)
  [19]=>
  int(26)
  [20]=>
  int(27)
  [21]=>
  int(28)
  [22]=>
  int(29)
  [23]=>
  int(3)
  [24]=>
  int(30)
  [25]=>
  int(31)
  [26]=>
  int(32)
  [27]=>
  int(33)
  [28]=>
  int(34)
  [29]=>
  int(35)
  [30]=>
  int(36)
  [31]=>
  int(37)
  [32]=>
  int(38)
  [33]=>
  int(39)
  [34]=>
  int(4)
  [35]=>
  int(40)
  [36]=>
  int(41)
  [37]=>
  int(42)
  [38]=>
  int(43)
  [39]=>
  int(44)
  [40]=>
  int(45)
  [41]=>
  int(46)
  [42]=>
  int(47)
  [43]=>
  int(48)
  [44]=>
  int(49)
  [45]=>
  int(5)
  [46]=>
  int(50)
  [47]=>
  int(51)
  [48]=>
  int(52)
  [49]=>
  int(53)
  [50]=>
  int(54)
  [51]=>
  int(55)
  [52]=>
  int(56)
  [53]=>
  int(57)
  [54]=>
  int(58)
  [55]=>
  int(59)
  [56]=>
  int(6)
  [57]=>
  int(60)
  [58]=>
  int(61)
  [59]=>
  int(62)
  [60]=>
  int(63)
  [61]=>
  int(64)
  [62]=>
  int(65)
  [63]=>
  int(66)
  [64]=>
  int(67)
  [65]=>
  int(68)
  [66]=>
  int(69)
  [67]=>
  int(7)
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
  int(8)
  [79]=>
  int(80)
  [80]=>
  int(81)
  [81]=>
  int(82)
  [82]=>
  int(83)
  [83]=>
  int(84)
  [84]=>
  int(85)
  [85]=>
  int(86)
  [86]=>
  int(87)
  [87]=>
  int(88)
  [88]=>
  int(89)
  [89]=>
  int(9)
  [90]=>
  int(90)
  [91]=>
  int(91)
  [92]=>
  int(92)
}
array(1) {
  [-2147483648]=>
  int(-65)
}
array(6) {
  [0]=>
  int(-63)
  [1]=>
  int(-64)
  [2]=>
  int(-65)
  [3]=>
  int(63)
  [4]=>
  int(64)
  [5]=>
  int(65)
}
array(5) {
  [0]=>
  int(-2147483646)
  [1]=>
  int(-2147483647)
  [2]=>
  float(-2147483648)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
}
array(6) {
  [0]=>
  int(-2147483646)
  [1]=>
  int(-2147483647)
  [2]=>
  float(-2147483648)
  [3]=>
  int(2147483646)
  [4]=>
  int(2147483647)
  [5]=>
  float(2147483648)
}
array(0) {
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            int(0)
          }
        }
      }
    }
  }
}
