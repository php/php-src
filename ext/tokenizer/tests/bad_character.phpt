--TEST--
token_get_all() produces T_BAD_CHARACTER for unexpected characters
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--FILE--
<?php

$codes = [
    "<?php \0 foo",
    "<?php \1 bar",
    "<?php \1\2 bar \3",
];

foreach ($codes as $code) {
    foreach (token_get_all($code) as $token) {
        if (is_array($token)) {
            echo token_name($token[0]), " ", strlen($token[1]), "\n";
        } else {
            echo $token, "\n";
        }
    }
    echo "\n";
}

?>
--EXPECT--
T_OPEN_TAG 6
T_BAD_CHARACTER 1
T_WHITESPACE 1
T_STRING 3

T_OPEN_TAG 6
T_BAD_CHARACTER 1
T_WHITESPACE 1
T_STRING 3

T_OPEN_TAG 6
T_BAD_CHARACTER 1
T_BAD_CHARACTER 1
T_WHITESPACE 1
T_STRING 3
T_WHITESPACE 1
T_BAD_CHARACTER 1
