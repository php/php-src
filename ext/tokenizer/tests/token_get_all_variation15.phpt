--TEST--
Test token_get_all() function : usage variations - heredoc string for 'source' 
--INI--
short_open_tag=On
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Testing token_get_all() with heredoc 'source' string with all different types of token and heredoc string within
 *     <<<EOT - T_START_HEREDOC(371)
 *     EOT - T_END_HEREDOC(372)
*/

echo "*** Testing token_get_all() : with heredoc source string ***\n";

$source = <<<EOT
<?= 
  \$a = 2;
  \$b = 1;
  \$c = <<<EOS
  This is to test 
  heredoc string
EOS;
  echo \$a + \$b;
  function myFunction(\$a)
  {
    var_dump(\$a);
  }
  if(\$b < 10) {
    \$b++;
  }
  else
    \$b--;
  while(\$a > 0) {
    echo "*";
    \$a--;
  }
  myFunction(10);
?>
EOT;
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : with heredoc source string ***
array(103) {
  [0]=>
  array(3) {
    [0]=>
    int(368)
    [1]=>
    string(3) "<?="
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(4) " 
  "
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [4]=>
  string(1) "="
  [5]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [6]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "2"
    [2]=>
    int(2)
  }
  [7]=>
  string(1) ";"
  [8]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(2)
  }
  [9]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(3)
  }
  [10]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(3)
  }
  [11]=>
  string(1) "="
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(3)
  }
  [13]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "1"
    [2]=>
    int(3)
  }
  [14]=>
  string(1) ";"
  [15]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(3)
  }
  [16]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$c"
    [2]=>
    int(4)
  }
  [17]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [18]=>
  string(1) "="
  [19]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [20]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(7) "<<<EOS
"
    [2]=>
    int(4)
  }
  [21]=>
  array(3) {
    [0]=>
    int(314)
    [1]=>
    string(36) "  This is to test 
  heredoc string
"
    [2]=>
    int(5)
  }
  [22]=>
  array(3) {
    [0]=>
    int(372)
    [1]=>
    string(3) "EOS"
    [2]=>
    int(7)
  }
  [23]=>
  string(1) ";"
  [24]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(7)
  }
  [25]=>
  array(3) {
    [0]=>
    int(316)
    [1]=>
    string(4) "echo"
    [2]=>
    int(8)
  }
  [26]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [27]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(8)
  }
  [28]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [29]=>
  string(1) "+"
  [30]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(8)
  }
  [31]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(8)
  }
  [32]=>
  string(1) ";"
  [33]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(8)
  }
  [34]=>
  array(3) {
    [0]=>
    int(333)
    [1]=>
    string(8) "function"
    [2]=>
    int(9)
  }
  [35]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [36]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "myFunction"
    [2]=>
    int(9)
  }
  [37]=>
  string(1) "("
  [38]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(9)
  }
  [39]=>
  string(1) ")"
  [40]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(9)
  }
  [41]=>
  string(1) "{"
  [42]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(10)
  }
  [43]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(8) "var_dump"
    [2]=>
    int(11)
  }
  [44]=>
  string(1) "("
  [45]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(11)
  }
  [46]=>
  string(1) ")"
  [47]=>
  string(1) ";"
  [48]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
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
    string(3) "
  "
    [2]=>
    int(12)
  }
  [51]=>
  array(3) {
    [0]=>
    int(301)
    [1]=>
    string(2) "if"
    [2]=>
    int(13)
  }
  [52]=>
  string(1) "("
  [53]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(13)
  }
  [54]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(13)
  }
  [55]=>
  string(1) "<"
  [56]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(13)
  }
  [57]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(2) "10"
    [2]=>
    int(13)
  }
  [58]=>
  string(1) ")"
  [59]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(13)
  }
  [60]=>
  string(1) "{"
  [61]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(13)
  }
  [62]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(14)
  }
  [63]=>
  array(3) {
    [0]=>
    int(297)
    [1]=>
    string(2) "++"
    [2]=>
    int(14)
  }
  [64]=>
  string(1) ";"
  [65]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(14)
  }
  [66]=>
  string(1) "}"
  [67]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(15)
  }
  [68]=>
  array(3) {
    [0]=>
    int(303)
    [1]=>
    string(4) "else"
    [2]=>
    int(16)
  }
  [69]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(16)
  }
  [70]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(17)
  }
  [71]=>
  array(3) {
    [0]=>
    int(296)
    [1]=>
    string(2) "--"
    [2]=>
    int(17)
  }
  [72]=>
  string(1) ";"
  [73]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(17)
  }
  [74]=>
  array(3) {
    [0]=>
    int(318)
    [1]=>
    string(5) "while"
    [2]=>
    int(18)
  }
  [75]=>
  string(1) "("
  [76]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(18)
  }
  [77]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(18)
  }
  [78]=>
  string(1) ">"
  [79]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(18)
  }
  [80]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "0"
    [2]=>
    int(18)
  }
  [81]=>
  string(1) ")"
  [82]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(18)
  }
  [83]=>
  string(1) "{"
  [84]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(18)
  }
  [85]=>
  array(3) {
    [0]=>
    int(316)
    [1]=>
    string(4) "echo"
    [2]=>
    int(19)
  }
  [86]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(19)
  }
  [87]=>
  array(3) {
    [0]=>
    int(315)
    [1]=>
    string(3) ""*""
    [2]=>
    int(19)
  }
  [88]=>
  string(1) ";"
  [89]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(5) "
    "
    [2]=>
    int(19)
  }
  [90]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(20)
  }
  [91]=>
  array(3) {
    [0]=>
    int(296)
    [1]=>
    string(2) "--"
    [2]=>
    int(20)
  }
  [92]=>
  string(1) ";"
  [93]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(20)
  }
  [94]=>
  string(1) "}"
  [95]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(3) "
  "
    [2]=>
    int(21)
  }
  [96]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(10) "myFunction"
    [2]=>
    int(22)
  }
  [97]=>
  string(1) "("
  [98]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(2) "10"
    [2]=>
    int(22)
  }
  [99]=>
  string(1) ")"
  [100]=>
  string(1) ";"
  [101]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) "
"
    [2]=>
    int(22)
  }
  [102]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(23)
  }
}
Done
