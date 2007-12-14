--TEST--
Test token_get_all() function : usage variations - with function constructs
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Testing token_get_all() with different function keywords
 *   function - T_FUNCTION(333), return - T_RETURN(335)
 *   different functions: 
 *     include() - T_INCLUDE(262), print() - T_PRINT(266), 
 *     isset() - T_ISSET(349), list() - T_LIST(358), 
 *     require() - T_REQUIRE(259), empty() - T_EMPTY(350), 
 *     declare() - T_DECLARE(324), array() - T_ARRAY(359), 
 *      __halt_compiler() - T_HALT_COMPILER(351)
*/

echo "*** Testing token_get_all() : with different function constructs ***\n";

$source = '<?php
declare(VALUE=100);
include("addfile.php");
require("sumfile.php");

function myFunction($a)
{
  if($a % 2)
    return 1;
  else
    exit;
}

$a = VALUE;
$b = 20;
$c = array(1,2);
$b >>= 2;

if($b <= 0)
  die;
else
  print($b);

list($value1,$value2) = $c;
if(empty($value1) && !isset($value1)) {
  __halt_compiler();
}
?>';
$tokens =  token_get_all($source);
var_dump($tokens);

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : with different function constructs ***
array(142) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
    [1]=>
    string(6) "<?php
"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(324)
    [1]=>
    string(7) "declare"
    [2]=>
    int(2)
  }
  [2]=>
  string(1) "("
  [3]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(5) "VALUE"
    [2]=>
    int(2)
  }
  [4]=>
  string(1) "="
  [5]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(3) "100"
    [2]=>
    int(2)
  }
  [6]=>
  string(1) ")"
  [7]=>
  string(1) ";"
  [8]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [9]=>
  array(3) {
    [0]=>
    int(262)
    [1]=>
    string(7) "include"
    [2]=>
    int(3)
  }
  [10]=>
  string(1) "("
  [11]=>
  array(3) {
    [0]=>
    int(315)
    [1]=>
    string(13) ""addfile.php""
    [2]=>
    int(3)
  }
  [12]=>
  string(1) ")"
  [13]=>
  string(1) ";"
  [14]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(3)
  }
  [15]=>
  array(3) {
    [0]=>
    int(259)
    [1]=>
    string(7) "require"
    [2]=>
    int(4)
  }
  [16]=>
  string(1) "("
  [17]=>
  array(3) {
    [0]=>
    int(315)
    [1]=>
    string(13) ""sumfile.php""
    [2]=>
    int(4)
  }
  [18]=>
  string(1) ")"
  [19]=>
  string(1) ";"
  [20]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "

"
    [2]=>
    int(4)
  }
  [21]=>
  array(3) {
    [0]=>
    int(333)
    [1]=>
    string(8) "function"
    [2]=>
    int(6)
  }
  [22]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [23]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "myFunction"
    [2]=>
    int(6)
  }
  [24]=>
  string(1) "("
  [25]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(6)
  }
  [26]=>
  string(1) ")"
  [27]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(6)
  }
  [28]=>
  string(1) "{"
  [29]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(7)
  }
  [30]=>
  array(3) {
    [0]=>
    int(301)
    [1]=>
    string(2) "if"
    [2]=>
    int(8)
  }
  [31]=>
  string(1) "("
  [32]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(8)
  }
  [33]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [34]=>
  string(1) "%"
  [35]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [36]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "2"
    [2]=>
    int(8)
  }
  [37]=>
  string(1) ")"
  [38]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(8)
  }
  [39]=>
  array(3) {
    [0]=>
    int(335)
    [1]=>
    string(6) "return"
    [2]=>
    int(9)
  }
  [40]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [41]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "1"
    [2]=>
    int(9)
  }
  [42]=>
  string(1) ";"
  [43]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(9)
  }
  [44]=>
  array(3) {
    [0]=>
    int(303)
    [1]=>
    string(4) "else"
    [2]=>
    int(10)
  }
  [45]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(10)
  }
  [46]=>
  array(3) {
    [0]=>
    int(300)
    [1]=>
    string(4) "exit"
    [2]=>
    int(11)
  }
  [47]=>
  string(1) ";"
  [48]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(11)
  }
  [49]=>
  string(1) "}"
  [50]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "

"
    [2]=>
    int(12)
  }
  [51]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(14)
  }
  [52]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(14)
  }
  [53]=>
  string(1) "="
  [54]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(14)
  }
  [55]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(5) "VALUE"
    [2]=>
    int(14)
  }
  [56]=>
  string(1) ";"
  [57]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(14)
  }
  [58]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(15)
  }
  [59]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [60]=>
  string(1) "="
  [61]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(15)
  }
  [62]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(2) "20"
    [2]=>
    int(15)
  }
  [63]=>
  string(1) ";"
  [64]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(15)
  }
  [65]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$c"
    [2]=>
    int(16)
  }
  [66]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [67]=>
  string(1) "="
  [68]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [69]=>
  array(3) {
    [0]=>
    int(359)
    [1]=>
    string(5) "array"
    [2]=>
    int(16)
  }
  [70]=>
  string(1) "("
  [71]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "1"
    [2]=>
    int(16)
  }
  [72]=>
  string(1) ","
  [73]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "2"
    [2]=>
    int(16)
  }
  [74]=>
  string(1) ")"
  [75]=>
  string(1) ";"
  [76]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(16)
  }
  [77]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(17)
  }
  [78]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(17)
  }
  [79]=>
  array(3) {
    [0]=>
    int(267)
    [1]=>
    string(3) ">>="
    [2]=>
    int(17)
  }
  [80]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(17)
  }
  [81]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "2"
    [2]=>
    int(17)
  }
  [82]=>
  string(1) ";"
  [83]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "

"
    [2]=>
    int(17)
  }
  [84]=>
  array(3) {
    [0]=>
    int(301)
    [1]=>
    string(2) "if"
    [2]=>
    int(19)
  }
  [85]=>
  string(1) "("
  [86]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(19)
  }
  [87]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(19)
  }
  [88]=>
  array(3) {
    [0]=>
    int(285)
    [1]=>
    string(2) "<="
    [2]=>
    int(19)
  }
  [89]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(19)
  }
  [90]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "0"
    [2]=>
    int(19)
  }
  [91]=>
  string(1) ")"
  [92]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(19)
  }
  [93]=>
  array(3) {
    [0]=>
    int(300)
    [1]=>
    string(3) "die"
    [2]=>
    int(20)
  }
  [94]=>
  string(1) ";"
  [95]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(20)
  }
  [96]=>
  array(3) {
    [0]=>
    int(303)
    [1]=>
    string(4) "else"
    [2]=>
    int(21)
  }
  [97]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(21)
  }
  [98]=>
  array(3) {
    [0]=>
    int(266)
    [1]=>
    string(5) "print"
    [2]=>
    int(22)
  }
  [99]=>
  string(1) "("
  [100]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(22)
  }
  [101]=>
  string(1) ")"
  [102]=>
  string(1) ";"
  [103]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "

"
    [2]=>
    int(22)
  }
  [104]=>
  array(3) {
    [0]=>
    int(358)
    [1]=>
    string(4) "list"
    [2]=>
    int(24)
  }
  [105]=>
  string(1) "("
  [106]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(7) "$value1"
    [2]=>
    int(24)
  }
  [107]=>
  string(1) ","
  [108]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(7) "$value2"
    [2]=>
    int(24)
  }
  [109]=>
  string(1) ")"
  [110]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(24)
  }
  [111]=>
  string(1) "="
  [112]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(24)
  }
  [113]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$c"
    [2]=>
    int(24)
  }
  [114]=>
  string(1) ";"
  [115]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(24)
  }
  [116]=>
  array(3) {
    [0]=>
    int(301)
    [1]=>
    string(2) "if"
    [2]=>
    int(25)
  }
  [117]=>
  string(1) "("
  [118]=>
  array(3) {
    [0]=>
    int(350)
    [1]=>
    string(5) "empty"
    [2]=>
    int(25)
  }
  [119]=>
  string(1) "("
  [120]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(7) "$value1"
    [2]=>
    int(25)
  }
  [121]=>
  string(1) ")"
  [122]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(25)
  }
  [123]=>
  array(3) {
    [0]=>
    int(279)
    [1]=>
    string(2) "&&"
    [2]=>
    int(25)
  }
  [124]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(25)
  }
  [125]=>
  string(1) "!"
  [126]=>
  array(3) {
    [0]=>
    int(349)
    [1]=>
    string(5) "isset"
    [2]=>
    int(25)
  }
  [127]=>
  string(1) "("
  [128]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(7) "$value1"
    [2]=>
    int(25)
  }
  [129]=>
  string(1) ")"
  [130]=>
  string(1) ")"
  [131]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(25)
  }
  [132]=>
  string(1) "{"
  [133]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(25)
  }
  [134]=>
  array(3) {
    [0]=>
    int(351)
    [1]=>
    string(15) "__halt_compiler"
    [2]=>
    int(26)
  }
  [135]=>
  string(1) "("
  [136]=>
  string(1) ")"
  [137]=>
  string(1) ";"
  [138]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(26)
  }
  [139]=>
  string(1) "}"
  [140]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(27)
  }
  [141]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(28)
  }
}
Done
