--TEST--
PhpToken instance methods
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--FILE--
<?php

$code = <<<'PHP'
<?php
// comment
/** comment */
function foo() {
    echo "bar";
}
PHP;

// Token names and ignorability.
$tokens = PhpToken::tokenize($code);
foreach ($tokens as $i => $token) {
    printf("[%2d] %-26s %s\n", $i, $token->getTokenName(),
        $token->isIgnorable() ? "ignorable" : "meaningful");
}

// is() variations
$token = $tokens[5];

echo "\nSuccess:\n";
var_dump($token->is(T_FUNCTION));
var_dump($token->is('function'));
var_dump($token->is(['class', T_FUNCTION]));
var_dump($token->is([T_CLASS, 'function']));

echo "\nFailure:\n";
var_dump($token->is(T_CLASS));
var_dump($token->is('class'));
var_dump($token->is(['class', T_TRAIT]));
var_dump($token->is([T_CLASS, 'trait']));

echo "\nError:\n";
try {
    $token->is(3.141);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $token->is([3.141]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

unset($token->id);
unset($token->text);
try {
    $token->is(T_FUNCTION);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $token->is('function');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $token->is([T_FUNCTION]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $token->is(['function']);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\nName of unknown token:\n";
$token = new PhpToken(100000, "foo");
var_dump($token->getTokenName());

?>
--EXPECT--
[ 0] T_OPEN_TAG                 ignorable
[ 1] T_COMMENT                  ignorable
[ 2] T_WHITESPACE               ignorable
[ 3] T_DOC_COMMENT              ignorable
[ 4] T_WHITESPACE               ignorable
[ 5] T_FUNCTION                 meaningful
[ 6] T_WHITESPACE               ignorable
[ 7] T_STRING                   meaningful
[ 8] (                          meaningful
[ 9] )                          meaningful
[10] T_WHITESPACE               ignorable
[11] {                          meaningful
[12] T_WHITESPACE               ignorable
[13] T_ECHO                     meaningful
[14] T_WHITESPACE               ignorable
[15] T_CONSTANT_ENCAPSED_STRING meaningful
[16] ;                          meaningful
[17] T_WHITESPACE               ignorable
[18] }                          meaningful

Success:
bool(true)
bool(true)
bool(true)
bool(true)

Failure:
bool(false)
bool(false)
bool(false)
bool(false)

Error:
PhpToken::is(): Argument #1 ($kind) must be of type string|int|array, float given
PhpToken::is(): Argument #1 ($kind) must only have elements of type string|int, float given
Typed property PhpToken::$id must not be accessed before initialization
Typed property PhpToken::$text must not be accessed before initialization
Typed property PhpToken::$id must not be accessed before initialization
Typed property PhpToken::$text must not be accessed before initialization

Name of unknown token:
NULL
