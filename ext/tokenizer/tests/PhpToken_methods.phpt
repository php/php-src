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
$tokens = PhpToken::getAll($code);
foreach ($tokens as $i => $token) {
    printf("[%2d] %-26s %s\n", $i, $token->getTokenName(),
        $token->isIgnorable() ? "ignorable" : "meaningful");
}

// is() variations

echo "\nSuccess:\n";
var_dump($tokens[4]->is(T_FUNCTION));
var_dump($tokens[4]->is('function'));
var_dump($tokens[4]->is(['class', T_FUNCTION]));
var_dump($tokens[4]->is([T_CLASS, 'function']));

echo "\nFailure:\n";
var_dump($tokens[4]->is(T_CLASS));
var_dump($tokens[4]->is('class'));
var_dump($tokens[4]->is(['class', T_TRAIT]));
var_dump($tokens[4]->is([T_CLASS, 'trait']));

echo "\nError:\n";
try {
    $tokens[4]->is(3.141);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $tokens[4]->is([3.141]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

unset($tokens[4]->id);
unset($tokens[4]->text);
try {
    $tokens[4]->is(T_FUNCTION);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $tokens[4]->is('function');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $tokens[4]->is([T_FUNCTION]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $tokens[4]->is(['function']);
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
[ 2] T_DOC_COMMENT              ignorable
[ 3] T_WHITESPACE               ignorable
[ 4] T_FUNCTION                 meaningful
[ 5] T_WHITESPACE               ignorable
[ 6] T_STRING                   meaningful
[ 7] (                          meaningful
[ 8] )                          meaningful
[ 9] T_WHITESPACE               ignorable
[10] {                          meaningful
[11] T_WHITESPACE               ignorable
[12] T_ECHO                     meaningful
[13] T_WHITESPACE               ignorable
[14] T_CONSTANT_ENCAPSED_STRING meaningful
[15] ;                          meaningful
[16] T_WHITESPACE               ignorable
[17] }                          meaningful

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
