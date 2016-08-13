--TEST--
Parse errors during token_get_all() with TOKEN_PARSE flag
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

try {
    token_get_all('<?php invalid code;', TOKEN_PARSE);
} catch (ParseError $e) {
    echo $e->getMessage(), PHP_EOL;
}

echo "Done";

?>
--EXPECT--
syntax error, unexpected 'code' (T_STRING)
Done
