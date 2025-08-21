--TEST--
Bug #19507: Recursive tokenization during `token_get_all` result on error
--EXTENSIONS--
tokenizer
--FILE--
<?php

set_error_handler(function (int $errno, string $msg) {
    eval('123;');
    echo 'error handler called: ', $msg, "\n";
});


$tokens = PhpToken::tokenize('<?php (double) $x;', TOKEN_PARSE);
foreach ($tokens as $token) {
    echo $token->getTokenName(), "\n";
}

--EXPECT--
error handler called: Non-canonical cast (double) is deprecated, use the (float) cast instead
T_OPEN_TAG
T_LNUMBER
;
T_DOUBLE_CAST
T_WHITESPACE
T_VARIABLE
;
