--TEST--
Handling of "namespace as" in TOKEN_PARSE mode
--FILE--
<?php

$code = <<<'CODE'
<?php
class C {
    use A {
        namespace as bar;
    }
}
CODE;

$tokens = PhpToken::tokenize($code, TOKEN_PARSE);
foreach ($tokens as $token) {
    echo "{$token->getTokenName()}: \"$token->text\"\n";
}

?>
--EXPECT--
T_OPEN_TAG: "<?php
"
T_CLASS: "class"
T_WHITESPACE: " "
T_STRING: "C"
T_WHITESPACE: " "
{: "{"
T_WHITESPACE: "
    "
T_USE: "use"
T_WHITESPACE: " "
T_STRING: "A"
T_WHITESPACE: " "
{: "{"
T_WHITESPACE: "
        "
T_STRING: "namespace"
T_WHITESPACE: " "
T_AS: "as"
T_WHITESPACE: " "
T_STRING: "bar"
;: ";"
T_WHITESPACE: "
    "
}: "}"
T_WHITESPACE: "
"
}: "}"
