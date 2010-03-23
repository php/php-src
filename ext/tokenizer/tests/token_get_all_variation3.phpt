--TEST--
Test token_get_all() function : usage variations - with logical operators
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Passing 'source' argument with different logical operators to test them for tokens
 *   and - T_AND_LOGICAL_AND(265), 
 *   or - T_LOGICAL_OR(263), 
 *   xor - T_LOGICAL_XOR(264), 
 *   && - T_BOOLEAN_AND(279), 
 *   || - T_BOOLEAN_OR(278)
*/

echo "*** Testing token_get_all() : 'source' string with different logical operators ***\n";

// logical operators : 'and', 'or', 'xor', '&&', '||' 
$source = array (
  '<?php $a = 1 and 024; ?>',
  '<?php $b = $b or 0X1E; ?>',
  '<?php $c = $a xor $b; ?>',
  '<?php $a = $b && 2; ?>',
  '<?php $b = $b || 1; ?>'
);
for($count = 0; $count < count($source); $count++) {
  echo "-- Iteration ".($count + 1)." --\n";
  var_dump( token_get_all($source[$count]));
}

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : 'source' string with different logical operators ***
-- Iteration 1 --
array(13) {
  [0]=>
  array(3) {
    [0]=>
    int(368)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [3]=>
  string(1) "="
  [4]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
  [6]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  array(3) {
    [0]=>
    int(265)
    [1]=>
    string(3) "and"
    [2]=>
    int(1)
  }
  [8]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(3) "024"
    [2]=>
    int(1)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 2 --
array(13) {
  [0]=>
  array(3) {
    [0]=>
    int(368)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [3]=>
  string(1) "="
  [4]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [6]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  array(3) {
    [0]=>
    int(263)
    [1]=>
    string(2) "or"
    [2]=>
    int(1)
  }
  [8]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(4) "0X1E"
    [2]=>
    int(1)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 3 --
array(13) {
  [0]=>
  array(3) {
    [0]=>
    int(368)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$c"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [3]=>
  string(1) "="
  [4]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [6]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  array(3) {
    [0]=>
    int(264)
    [1]=>
    string(3) "xor"
    [2]=>
    int(1)
  }
  [8]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 4 --
array(13) {
  [0]=>
  array(3) {
    [0]=>
    int(368)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [3]=>
  string(1) "="
  [4]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [6]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  array(3) {
    [0]=>
    int(279)
    [1]=>
    string(2) "&&"
    [2]=>
    int(1)
  }
  [8]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "2"
    [2]=>
    int(1)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 5 --
array(13) {
  [0]=>
  array(3) {
    [0]=>
    int(368)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [3]=>
  string(1) "="
  [4]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [6]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  array(3) {
    [0]=>
    int(278)
    [1]=>
    string(2) "||"
    [2]=>
    int(1)
  }
  [8]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(371)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
Done
