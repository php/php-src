--TEST--
Parse errors during token_get_all() with TOKEN_PARSE flag
--EXTENSIONS--
tokenizer
--FILE--
<?php

try {
    token_get_all('<?php invalid code;', TOKEN_PARSE);
} catch (ParseError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Done";

?>
--EXPECT--
ParseError: syntax error, unexpected identifier "code"
Done
