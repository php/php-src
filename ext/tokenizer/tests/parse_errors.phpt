--TEST--
Parse errors during token_get_all()
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

function test_parse_error($code) {
    try {
        var_dump(token_get_all($code));
    } catch (ParseException $e) {
        echo $e->getMessage(), "\n";
    }
}

test_parse_error('<?php var_dump(078);');
test_parse_error('<?php var_dump("\u{xyz}");');
test_parse_error('<?php var_dump("\u{ffffff}");');
test_parse_error('<?php var_dump(078 + 078);');

?>
--EXPECT--
Invalid numeric literal
Invalid UTF-8 codepoint escape sequence
Invalid UTF-8 codepoint escape sequence: Codepoint too large
Invalid numeric literal
