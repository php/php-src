--TEST--
Tokenization of namespaced names
--EXTENSIONS--
tokenizer
--FILE--
<?php

$code = <<<'CODE'
<?php
Foo
Foo\Bar
\Foo\Bar
namespace\Foo
Foo \ Bar
CODE;

foreach (PhpToken::tokenize($code) as $token) {
    echo "{$token->getTokenName()}: \"$token->text\"\n";
}

?>
--EXPECT--
T_OPEN_TAG: "<?php
"
T_STRING: "Foo"
T_WHITESPACE: "
"
T_NAME_QUALIFIED: "Foo\Bar"
T_WHITESPACE: "
"
T_NAME_FULLY_QUALIFIED: "\Foo\Bar"
T_WHITESPACE: "
"
T_NAME_RELATIVE: "namespace\Foo"
T_WHITESPACE: "
"
T_STRING: "Foo"
T_WHITESPACE: " "
T_NS_SEPARATOR: "\"
T_WHITESPACE: " "
T_STRING: "Bar"
