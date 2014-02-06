--TEST--
Bug 60097: token_get_all fails to lex nested heredoc
--FILE--
<?php

var_dump(token_get_all('<?php
<<<DOC1
{$s(<<<DOC2
DOC2
)}
DOC1;
'));

?>
--EXPECT--
array(14) {
  [0]=>
  array(3) {
    [0]=>
    int(376)
    [1]=>
    string(6) "<?php
"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(380)
    [1]=>
    string(8) "<<<DOC1
"
    [2]=>
    int(2)
  }
  [2]=>
  array(3) {
    [0]=>
    int(383)
    [1]=>
    string(1) "{"
    [2]=>
    int(3)
  }
  [3]=>
  array(3) {
    [0]=>
    int(312)
    [1]=>
    string(2) "$s"
    [2]=>
    int(3)
  }
  [4]=>
  string(1) "("
  [5]=>
  array(3) {
    [0]=>
    int(380)
    [1]=>
    string(8) "<<<DOC2
"
    [2]=>
    int(3)
  }
  [6]=>
  array(3) {
    [0]=>
    int(381)
    [1]=>
    string(4) "DOC2"
    [2]=>
    int(4)
  }
  [7]=>
  array(3) {
    [0]=>
    int(379)
    [1]=>
    string(1) "
"
    [2]=>
    int(4)
  }
  [8]=>
  string(1) ")"
  [9]=>
  string(1) "}"
  [10]=>
  array(3) {
    [0]=>
    int(317)
    [1]=>
    string(1) "
"
    [2]=>
    int(5)
  }
  [11]=>
  array(3) {
    [0]=>
    int(381)
    [1]=>
    string(4) "DOC1"
    [2]=>
    int(6)
  }
  [12]=>
  string(1) ";"
  [13]=>
  array(3) {
    [0]=>
    int(379)
    [1]=>
    string(1) "
"
    [2]=>
    int(6)
  }
}
