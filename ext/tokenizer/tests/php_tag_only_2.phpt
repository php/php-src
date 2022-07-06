--TEST--
Tokenization of only the <?php tag
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--INI--
short_open_tag=0
--FILE--
<?php

foreach (token_get_all("<?php") as $token) {
    echo token_name($token[0]), "\n";
}
echo "\n";
foreach (token_get_all("Foobar<?php") as $token) {
    echo token_name($token[0]), "\n";
}

?>
--EXPECT--
T_OPEN_TAG

T_INLINE_HTML
T_OPEN_TAG
