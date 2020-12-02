--TEST--
Bug #80462: Nullsafe operator tokenize with TOKEN_PARSE flag fails
--FILE--
<?php

foreach (PhpToken::tokenize('<?php $foo = $a?->b();', TOKEN_PARSE) as $token) {
    echo $token->getTokenName(), "\n";
}

?>
--EXPECT--
T_OPEN_TAG
T_VARIABLE
T_WHITESPACE
=
T_WHITESPACE
T_VARIABLE
T_NULLSAFE_OBJECT_OPERATOR
T_STRING
(
)
;
