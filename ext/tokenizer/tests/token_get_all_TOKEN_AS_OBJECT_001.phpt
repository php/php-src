--TEST--
TOKEN_AS_OBJECT mode
--FILE--
<?php

$code = <<<'PHP'
<?php
function foo() {
    echo "bar";
}
PHP;
var_dump(token_get_all($code, TOKEN_AS_OBJECT));

?>
--EXPECT--
array(15) {
  [0]=>
  object(PhpToken)#1 (3) {
    ["type"]=>
    int(379)
    ["text"]=>
    string(6) "<?php
"
    ["line"]=>
    int(1)
  }
  [1]=>
  object(PhpToken)#2 (3) {
    ["type"]=>
    int(346)
    ["text"]=>
    string(8) "function"
    ["line"]=>
    int(2)
  }
  [2]=>
  object(PhpToken)#3 (3) {
    ["type"]=>
    int(382)
    ["text"]=>
    string(1) " "
    ["line"]=>
    int(2)
  }
  [3]=>
  object(PhpToken)#4 (3) {
    ["type"]=>
    int(319)
    ["text"]=>
    string(3) "foo"
    ["line"]=>
    int(2)
  }
  [4]=>
  object(PhpToken)#5 (3) {
    ["type"]=>
    int(40)
    ["text"]=>
    string(1) "("
    ["line"]=>
    int(2)
  }
  [5]=>
  object(PhpToken)#6 (3) {
    ["type"]=>
    int(41)
    ["text"]=>
    string(1) ")"
    ["line"]=>
    int(2)
  }
  [6]=>
  object(PhpToken)#7 (3) {
    ["type"]=>
    int(382)
    ["text"]=>
    string(1) " "
    ["line"]=>
    int(2)
  }
  [7]=>
  object(PhpToken)#8 (3) {
    ["type"]=>
    int(123)
    ["text"]=>
    string(1) "{"
    ["line"]=>
    int(2)
  }
  [8]=>
  object(PhpToken)#9 (3) {
    ["type"]=>
    int(382)
    ["text"]=>
    string(5) "
    "
    ["line"]=>
    int(2)
  }
  [9]=>
  object(PhpToken)#10 (3) {
    ["type"]=>
    int(328)
    ["text"]=>
    string(4) "echo"
    ["line"]=>
    int(3)
  }
  [10]=>
  object(PhpToken)#11 (3) {
    ["type"]=>
    int(382)
    ["text"]=>
    string(1) " "
    ["line"]=>
    int(3)
  }
  [11]=>
  object(PhpToken)#12 (3) {
    ["type"]=>
    int(323)
    ["text"]=>
    string(5) ""bar""
    ["line"]=>
    int(3)
  }
  [12]=>
  object(PhpToken)#13 (3) {
    ["type"]=>
    int(59)
    ["text"]=>
    string(1) ";"
    ["line"]=>
    int(3)
  }
  [13]=>
  object(PhpToken)#14 (3) {
    ["type"]=>
    int(382)
    ["text"]=>
    string(1) "
"
    ["line"]=>
    int(3)
  }
  [14]=>
  object(PhpToken)#15 (3) {
    ["type"]=>
    int(125)
    ["text"]=>
    string(1) "}"
    ["line"]=>
    int(4)
  }
}
