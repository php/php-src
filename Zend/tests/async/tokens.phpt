--TEST--
async/await tokens
--EXTENSIONS--
tokenizer
--FILE--
<?php
$code = '<?php async function f() { await g(); } async(1); async fn() => 1;';
foreach (PhpToken::tokenize($code) as $t) {
    if (in_array($t->text, ['async', 'await'], true)) {
        echo $t->text, " ", $t->getTokenName(), "\n";
    }
}
?>
--EXPECT--
async T_ASYNC
await T_AWAIT
async T_STRING
async T_ASYNC
