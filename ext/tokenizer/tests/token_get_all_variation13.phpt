--TEST--
Test token_get_all() function : usage variations - with class/object constructs
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Testing token_get_all() with different class/object keywords
 *   scope related : 
 *     static - T_STATIC(346), global - T_GLOBAL(340),
 *     private - T_PRIVATE(343), public - T_PUBLIC(341),
 *     protected - T_PROTECTED(342)
 *   class/object related : 
 *     var - T_VAR(347), abstract - T_ABSTRACT(345), 
 *     interface - T_INTERFACE(353), class - T_CLASS(352),
 *     extends - T_EXTENDS(354), implements - T_IMPLEMENTS(355), new - T_NEW(299)
*/

echo "*** Testing token_get_all() : with class/object constructs ***\n";

$source = '<?php
interface MyInterface
{
  public const var $var = 10;
}
abstract class MyClass
{
  private var $a;
  public var $b;
  protected var $c;
  static $d;
  final $e = 10;
  
  abstract public function myFunction($a);
}
class ChildClass extends MyClass implements MyInterface
{
  global $value;
  function myFunction($a)
  {
    $a = new ChildClass();
    if($a instanceof MyClass)
      echo "object created";
  }
}
ChildClass::myFunction(10);
?>';
$tokens =  token_get_all($source);
var_dump($tokens);

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : with class/object constructs ***
array(145) {
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
    int(353)
    [1]=>
    string(9) "interface"
    [2]=>
    int(2)
  }
  [2]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(11) "MyInterface"
    [2]=>
    int(2)
  }
  [4]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [5]=>
  string(1) "{"
  [6]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(3)
  }
  [7]=>
  array(3) {
    [0]=>
    int(341)
    [1]=>
    string(6) "public"
    [2]=>
    int(4)
  }
  [8]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [9]=>
  array(3) {
    [0]=>
    int(334)
    [1]=>
    string(5) "const"
    [2]=>
    int(4)
  }
  [10]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [11]=>
  array(3) {
    [0]=>
    int(347)
    [1]=>
    string(3) "var"
    [2]=>
    int(4)
  }
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [13]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(4) "$var"
    [2]=>
    int(4)
  }
  [14]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [15]=>
  string(1) "="
  [16]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [17]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(2) "10"
    [2]=>
    int(4)
  }
  [18]=>
  string(1) ";"
  [19]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [20]=>
  string(1) "}"
  [21]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [22]=>
  array(3) {
    [0]=>
    int(345)
    [1]=>
    string(8) "abstract"
    [2]=>
    int(6)
  }
  [23]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [24]=>
  array(3) {
    [0]=>
    int(352)
    [1]=>
    string(5) "class"
    [2]=>
    int(6)
  }
  [25]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(6)
  }
  [26]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(7) "MyClass"
    [2]=>
    int(6)
  }
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
    int(343)
    [1]=>
    string(7) "private"
    [2]=>
    int(8)
  }
  [31]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [32]=>
  array(3) {
    [0]=>
    int(347)
    [1]=>
    string(3) "var"
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
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(8)
  }
  [35]=>
  string(1) ";"
  [36]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(8)
  }
  [37]=>
  array(3) {
    [0]=>
    int(341)
    [1]=>
    string(6) "public"
    [2]=>
    int(9)
  }
  [38]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [39]=>
  array(3) {
    [0]=>
    int(347)
    [1]=>
    string(3) "var"
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
    int(309)
    [1]=>
    string(2) "$b"
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
    int(342)
    [1]=>
    string(9) "protected"
    [2]=>
    int(10)
  }
  [45]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [46]=>
  array(3) {
    [0]=>
    int(347)
    [1]=>
    string(3) "var"
    [2]=>
    int(10)
  }
  [47]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [48]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$c"
    [2]=>
    int(10)
  }
  [49]=>
  string(1) ";"
  [50]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(10)
  }
  [51]=>
  array(3) {
    [0]=>
    int(346)
    [1]=>
    string(6) "static"
    [2]=>
    int(11)
  }
  [52]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(11)
  }
  [53]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$d"
    [2]=>
    int(11)
  }
  [54]=>
  string(1) ";"
  [55]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(11)
  }
  [56]=>
  array(3) {
    [0]=>
    int(344)
    [1]=>
    string(5) "final"
    [2]=>
    int(12)
  }
  [57]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(12)
  }
  [58]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$e"
    [2]=>
    int(12)
  }
  [59]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(12)
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
    int(12)
  }
  [62]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(2) "10"
    [2]=>
    int(12)
  }
  [63]=>
  string(1) ";"
  [64]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(6) "
  
  "
    [2]=>
    int(12)
  }
  [65]=>
  array(3) {
    [0]=>
    int(345)
    [1]=>
    string(8) "abstract"
    [2]=>
    int(14)
  }
  [66]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(14)
  }
  [67]=>
  array(3) {
    [0]=>
    int(341)
    [1]=>
    string(6) "public"
    [2]=>
    int(14)
  }
  [68]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(14)
  }
  [69]=>
  array(3) {
    [0]=>
    int(333)
    [1]=>
    string(8) "function"
    [2]=>
    int(14)
  }
  [70]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(14)
  }
  [71]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "myFunction"
    [2]=>
    int(14)
  }
  [72]=>
  string(1) "("
  [73]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(14)
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
    int(14)
  }
  [77]=>
  string(1) "}"
  [78]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(15)
  }
  [79]=>
  array(3) {
    [0]=>
    int(352)
    [1]=>
    string(5) "class"
    [2]=>
    int(16)
  }
  [80]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [81]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "ChildClass"
    [2]=>
    int(16)
  }
  [82]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [83]=>
  array(3) {
    [0]=>
    int(354)
    [1]=>
    string(7) "extends"
    [2]=>
    int(16)
  }
  [84]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [85]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(7) "MyClass"
    [2]=>
    int(16)
  }
  [86]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [87]=>
  array(3) {
    [0]=>
    int(355)
    [1]=>
    string(10) "implements"
    [2]=>
    int(16)
  }
  [88]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(16)
  }
  [89]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(11) "MyInterface"
    [2]=>
    int(16)
  }
  [90]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(16)
  }
  [91]=>
  string(1) "{"
  [92]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(17)
  }
  [93]=>
  array(3) {
    [0]=>
    int(340)
    [1]=>
    string(6) "global"
    [2]=>
    int(18)
  }
  [94]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(18)
  }
  [95]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(6) "$value"
    [2]=>
    int(18)
  }
  [96]=>
  string(1) ";"
  [97]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(18)
  }
  [98]=>
  array(3) {
    [0]=>
    int(333)
    [1]=>
    string(8) "function"
    [2]=>
    int(19)
  }
  [99]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(19)
  }
  [100]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "myFunction"
    [2]=>
    int(19)
  }
  [101]=>
  string(1) "("
  [102]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(19)
  }
  [103]=>
  string(1) ")"
  [104]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(19)
  }
  [105]=>
  string(1) "{"
  [106]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(20)
  }
  [107]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(21)
  }
  [108]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(21)
  }
  [109]=>
  string(1) "="
  [110]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(21)
  }
  [111]=>
  array(3) {
    [0]=>
    int(299)
    [1]=>
    string(3) "new"
    [2]=>
    int(21)
  }
  [112]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(21)
  }
  [113]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "ChildClass"
    [2]=>
    int(21)
  }
  [114]=>
  string(1) "("
  [115]=>
  string(1) ")"
  [116]=>
  string(1) ";"
  [117]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(21)
  }
  [118]=>
  array(3) {
    [0]=>
    int(301)
    [1]=>
    string(2) "if"
    [2]=>
    int(22)
  }
  [119]=>
  string(1) "("
  [120]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(22)
  }
  [121]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(22)
  }
  [122]=>
  array(3) {
    [0]=>
    int(288)
    [1]=>
    string(10) "instanceof"
    [2]=>
    int(22)
  }
  [123]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(22)
  }
  [124]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(7) "MyClass"
    [2]=>
    int(22)
  }
  [125]=>
  string(1) ")"
  [126]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(7) "
      "
    [2]=>
    int(22)
  }
  [127]=>
  array(3) {
    [0]=>
    int(316)
    [1]=>
    string(4) "echo"
    [2]=>
    int(23)
  }
  [128]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(23)
  }
  [129]=>
  array(3) {
    [0]=>
    int(315)
    [1]=>
    string(16) ""object created""
    [2]=>
    int(23)
  }
  [130]=>
  string(1) ";"
  [131]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(23)
  }
  [132]=>
  string(1) "}"
  [133]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(24)
  }
  [134]=>
  string(1) "}"
  [135]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(25)
  }
  [136]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "ChildClass"
    [2]=>
    int(26)
  }
  [137]=>
  array(3) {
    [0]=>
    int(375)
    [1]=>
    string(2) "::"
    [2]=>
    int(26)
  }
  [138]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "myFunction"
    [2]=>
    int(26)
  }
  [139]=>
  string(1) "("
  [140]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(2) "10"
    [2]=>
    int(26)
  }
  [141]=>
  string(1) ")"
  [142]=>
  string(1) ";"
  [143]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(26)
  }
  [144]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(27)
  }
}
Done
