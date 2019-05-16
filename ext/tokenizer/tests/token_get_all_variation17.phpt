--TEST--
Test token_get_all() function : usage variations - with exception keywords
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Testing token_get_all() with different exception keywords
 *   try - T_TRY(336),
 *   catch - T_CATCH(337),
 *   throw - T_THROW(338)
*/

echo "*** Testing token_get_all() : with exception keywords ***\n";

$source = '<?php
function inverse($x)
{
  if($x == 0) {
    throw new Exception("Division by zero");
  else
    return 1/$x;
}
try {
  echo inverse(0);
  echo inverse(5);
} catch(Exception $e) {
    echo "caught exception:";
}
?>';
$tokens =  token_get_all($source);
var_dump($tokens);

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : with exception keywords ***
array(81) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php
"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(8) "function"
    [2]=>
    int(2)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(2)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "inverse"
    [2]=>
    int(2)
  }
  [4]=>
  string(1) "("
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$x"
    [2]=>
    int(2)
  }
  [6]=>
  string(1) ")"
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [8]=>
  string(1) "{"
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(%d)
  }
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
    [2]=>
    int(4)
  }
  [11]=>
  string(1) "("
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$x"
    [2]=>
    int(4)
  }
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [14]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "=="
    [2]=>
    int(4)
  }
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "0"
    [2]=>
    int(4)
  }
  [17]=>
  string(1) ")"
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(4)
  }
  [19]=>
  string(1) "{"
  [20]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "
    "
    [2]=>
    int(4)
  }
  [21]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "throw"
    [2]=>
    int(5)
  }
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "new"
    [2]=>
    int(5)
  }
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(5)
  }
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(9) "Exception"
    [2]=>
    int(5)
  }
  [26]=>
  string(1) "("
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(18) ""Division by zero""
    [2]=>
    int(5)
  }
  [28]=>
  string(1) ")"
  [29]=>
  string(1) ";"
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(5)
  }
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "else"
    [2]=>
    int(6)
  }
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "
    "
    [2]=>
    int(6)
  }
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "return"
    [2]=>
    int(7)
  }
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(7)
  }
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(7)
  }
  [36]=>
  string(1) "/"
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$x"
    [2]=>
    int(7)
  }
  [38]=>
  string(1) ";"
  [39]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(7)
  }
  [40]=>
  string(1) "}"
  [41]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(8)
  }
  [42]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "try"
    [2]=>
    int(9)
  }
  [43]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [44]=>
  string(1) "{"
  [45]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(9)
  }
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(10)
  }
  [47]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(10)
  }
  [48]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "inverse"
    [2]=>
    int(10)
  }
  [49]=>
  string(1) "("
  [50]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "0"
    [2]=>
    int(10)
  }
  [51]=>
  string(1) ")"
  [52]=>
  string(1) ";"
  [53]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "
  "
    [2]=>
    int(10)
  }
  [54]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(11)
  }
  [55]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(11)
  }
  [56]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "inverse"
    [2]=>
    int(11)
  }
  [57]=>
  string(1) "("
  [58]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "5"
    [2]=>
    int(11)
  }
  [59]=>
  string(1) ")"
  [60]=>
  string(1) ";"
  [61]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(11)
  }
  [62]=>
  string(1) "}"
  [63]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(12)
  }
  [64]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "catch"
    [2]=>
    int(12)
  }
  [65]=>
  string(1) "("
  [66]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(9) "Exception"
    [2]=>
    int(12)
  }
  [67]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(12)
  }
  [68]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$e"
    [2]=>
    int(12)
  }
  [69]=>
  string(1) ")"
  [70]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(12)
  }
  [71]=>
  string(1) "{"
  [72]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "
    "
    [2]=>
    int(12)
  }
  [73]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(13)
  }
  [74]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(13)
  }
  [75]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(19) ""caught exception:""
    [2]=>
    int(13)
  }
  [76]=>
  string(1) ";"
  [77]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(13)
  }
  [78]=>
  string(1) "}"
  [79]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(14)
  }
  [80]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(15)
  }
}
Done
