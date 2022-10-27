--TEST--
Bug #81342: New ampersand token parsing depends on new line after it
--EXTENSIONS--
tokenizer
--FILE--
<?php

$tokens = PhpToken::tokenize('<?php $x & $x; $x &
    $baz;
');
foreach ($tokens as $token) {
    echo $token->getTokenName(), "\n";
}

?>
--EXPECT--
T_OPEN_TAG
T_VARIABLE
T_WHITESPACE
T_AMPERSAND_FOLLOWED_BY_VAR_OR_VARARG
T_WHITESPACE
T_VARIABLE
;
T_WHITESPACE
T_VARIABLE
T_WHITESPACE
T_AMPERSAND_FOLLOWED_BY_VAR_OR_VARARG
T_WHITESPACE
T_VARIABLE
;
T_WHITESPACE
