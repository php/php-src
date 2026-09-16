--TEST--
GH-19507: Corrupted result after recursive tokenization during token_get_all() (error handler with throw)
--EXTENSIONS--
tokenizer
--FILE--
<?php

set_error_handler(function (int $errno, string $msg) {
    throw new RuntimeException('error handler called: ' . $msg);
});


$tokens = PhpToken::tokenize('<?php (double) $x;', TOKEN_PARSE);
foreach ($tokens as $token) {
    echo $token->getTokenName(), "\n";
}
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: error handler called: Non-canonical cast (double) is deprecated, use the (float) cast instead in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(%d, 'Non-canonical c...', '', 1)
#1 %s(%d): PhpToken::tokenize('<?php (double) ...', %d)
#2 {main}
  thrown in %s on line %d
