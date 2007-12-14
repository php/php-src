--TEST--
Test token_get_all() function : usage variations - with constant tokens
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Using different types of constants in 'source' string to check them for token
 * integer const - T_LNUMBER(305)
 * float/double/real const - T_DNUMBER(306)
 * string cosnt - T_CONSTANT_ESCAPED_STRING(315)
 * bool const (no tokens specified) - T_UNKNOWN(307)
 * null const (no tokens specified) - T_UNKNOWN(307)
*/

echo "*** Testing token_get_all() :  'source' string with different constants ***\n";

$a = 1;
$b = 0;

$source = array (
  // int const
  '<?php $a = 1 + 034; $b = $a + 0x3F; ?>', 
  
  // float const
  '<?php $a = 0.23E-2 + 0.43e2 + 0.5; ?>',

  // string const
  '<?php $a = "hello ".\'world\'; ?>',  

  // bool const
  "<?php \$a = (\$b)? true : false; ?>",  
  "<?php \$b = (\$a)? FALSE : TRUE; ?>",  

  // null const
  '<?php $b = null | NULL; ?>'
);
for($count = 0; $count < count($source); $count++) {
  echo "-- Iteration ".($count + 1)." --\n";
  var_dump( token_get_all($source[$count]));
}

echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() :  'source' string with different constants ***
-- Iteration 1 --
array(24) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
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
    int(370)
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
    int(370)
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
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  string(1) "+"
  [8]=>
  array(3) {
    [0]=>
    int(370)
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
    string(3) "034"
    [2]=>
    int(1)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [13]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [14]=>
  string(1) "="
  [15]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [16]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [17]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [18]=>
  string(1) "+"
  [19]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [20]=>
  array(3) {
    [0]=>
    int(305)
    [1]=>
    string(4) "0x3F"
    [2]=>
    int(1)
  }
  [21]=>
  string(1) ";"
  [22]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [23]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 2 --
array(17) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
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
    int(370)
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
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(306)
    [1]=>
    string(7) "0.23E-2"
    [2]=>
    int(1)
  }
  [6]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  string(1) "+"
  [8]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(306)
    [1]=>
    string(6) "0.43e2"
    [2]=>
    int(1)
  }
  [10]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [11]=>
  string(1) "+"
  [12]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [13]=>
  array(3) {
    [0]=>
    int(306)
    [1]=>
    string(3) "0.5"
    [2]=>
    int(1)
  }
  [14]=>
  string(1) ";"
  [15]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [16]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 3 --
array(11) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
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
    int(370)
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
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(315)
    [1]=>
    string(8) ""hello ""
    [2]=>
    int(1)
  }
  [6]=>
  string(1) "."
  [7]=>
  array(3) {
    [0]=>
    int(315)
    [1]=>
    string(7) "'world'"
    [2]=>
    int(1)
  }
  [8]=>
  string(1) ";"
  [9]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [10]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 4 --
array(18) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
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
    int(370)
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
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  string(1) "("
  [6]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [7]=>
  string(1) ")"
  [8]=>
  string(1) "?"
  [9]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [10]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(4) "true"
    [2]=>
    int(1)
  }
  [11]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  string(1) ":"
  [13]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [14]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(5) "false"
    [2]=>
    int(1)
  }
  [15]=>
  string(1) ";"
  [16]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [17]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 5 --
array(18) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
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
    int(370)
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
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  string(1) "("
  [6]=>
  array(3) {
    [0]=>
    int(309)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [7]=>
  string(1) ")"
  [8]=>
  string(1) "?"
  [9]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [10]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(5) "FALSE"
    [2]=>
    int(1)
  }
  [11]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  string(1) ":"
  [13]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [14]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(4) "TRUE"
    [2]=>
    int(1)
  }
  [15]=>
  string(1) ";"
  [16]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [17]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
-- Iteration 6 --
array(13) {
  [0]=>
  array(3) {
    [0]=>
    int(367)
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
    int(370)
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
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(4) "null"
    [2]=>
    int(1)
  }
  [6]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  string(1) "|"
  [8]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(307)
    [1]=>
    string(4) "NULL"
    [2]=>
    int(1)
  }
  [10]=>
  string(1) ";"
  [11]=>
  array(3) {
    [0]=>
    int(370)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [12]=>
  array(3) {
    [0]=>
    int(369)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
Done
