--TEST--
T_FROM not emitted for method calls
--EXTENSIONS--
tokenizer
--FILE--
<?php
$code = <<<'CODE'
<?php
$db->from('table');
CODE;

foreach (PhpToken::tokenize($code) as $token) {
    if ($token->text === 'from') {
        echo token_name($token->id) . " {$token->text}\n";
    }
}

?>
--EXPECT--
T_STRING from
