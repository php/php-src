--TEST--
T_FROM token recognition
--EXTENSIONS--
tokenizer
--FILE--
<?php

$code = <<<'CODE'
<?php
use MyClass from Foo\Bar;
CODE;

foreach (PhpToken::tokenize($code) as $token) {
    if ($token->id === T_FROM) {
        echo "T_FROM token found: \"{$token->text}\"\n";
    }
}

?>
--EXPECT--
T_FROM token found: "from"
