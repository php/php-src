--TEST--
Parse errors during token_get_all()
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

function test_parse_error($code) {
    try {
        var_dump(token_get_all($code, TOKEN_PARSE));
    } catch (ParseError $e) {
        echo $e->getMessage(), "\n";
    }

    foreach (token_get_all($code) as $token) {
        if (is_array($token)) {
            echo token_name($token[0]), " ($token[1])\n";
        } else {
            echo "$token\n";
        }
    }
    echo "\n";
}

test_parse_error('<?php var_dump(078);');
test_parse_error('<?php var_dump("\u{xyz}");');
test_parse_error('<?php var_dump("\u{ffffff}");');
test_parse_error('<?php var_dump(078 + 078);');

?>
--EXPECT--
Invalid numeric literal
T_OPEN_TAG (<?php )
T_STRING (var_dump)
(
T_LNUMBER (078)
)
;

Invalid UTF-8 codepoint escape sequence
T_OPEN_TAG (<?php )
T_STRING (var_dump)
(
T_CONSTANT_ENCAPSED_STRING ("\u{xyz}")
)
;

Invalid UTF-8 codepoint escape sequence: Codepoint too large
T_OPEN_TAG (<?php )
T_STRING (var_dump)
(
T_CONSTANT_ENCAPSED_STRING ("\u{ffffff}")
)
;

Invalid numeric literal
T_OPEN_TAG (<?php )
T_STRING (var_dump)
(
T_LNUMBER (078)
T_WHITESPACE ( )
+
T_WHITESPACE ( )
T_LNUMBER (078)
)
;
