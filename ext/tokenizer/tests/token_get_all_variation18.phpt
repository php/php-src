--TEST--
Test token_get_all() function : usage variations - with HTML code 
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Testing token_get_all() with source string containing HTML code with PHP
 *   HTML tags are considered to be T_INLINE_HTML(311)
*/

echo "*** Testing token_get_all() : 'source' string with HTML tags ***\n";

$source = '
<html>
<body>
  Testing HTML
</body>
</html>"

<?php 
  echo "php code with HTML";
?>';
var_dump( token_get_all($source));

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : 'source' string with HTML tags ***
array(9) {
  [0]=>
  array(3) {
    [0]=>
    int(311)
    [1]=>
    string(48) "
<html>
<body>
  Testing HTML
</body>
</html>"

"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(368)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(8)
  }
  [2]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(3) "
  "
    [2]=>
    int(8)
  }
  [3]=>
  array(3) {
    [0]=>
    int(316)
    [1]=>
    string(4) "echo"
    [2]=>
    int(9)
  }
  [4]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(9)
  }
  [5]=>
  array(3) {
    [0]=>
    int(315)
    [1]=>
    string(20) ""php code with HTML""
    [2]=>
    int(9)
  }
  [6]=>
  string(1) ";"
  [7]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) "
"
    [2]=>
    int(9)
  }
  [8]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "?>"
    [2]=>
    int(10)
  }
}
Done
