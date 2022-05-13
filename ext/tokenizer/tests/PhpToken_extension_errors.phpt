--TEST--
PhpToken extensions that throw during construction
--EXTENSIONS--
tokenizer
--FILE--
<?php

class MyPhpToken1 extends PhpToken {
    public $extra = UNKNOWN;
}

try {
    var_dump(MyPhpToken1::tokenize("<?php foo"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

abstract class MyPhpToken2 extends PhpToken {
}

try {
    var_dump(MyPhpToken2::tokenize("<?php foo"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant "UNKNOWN"
Cannot instantiate abstract class MyPhpToken2
