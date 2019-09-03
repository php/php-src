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
}

?>
--EXPECTF--
Warning: Unexpected character in input:  ' in %s on line %d
T_OPEN_TAG 6
T_BAD_CHARACTER 1
T_WHITESPACE 1
T_STRING 3

Warning: Unexpected character in input:  '%s' (ASCII=1) state=0 in %s on line %d
T_OPEN_TAG 6
T_BAD_CHARACTER 1
T_WHITESPACE 1
T_STRING 3

Warning: Unexpected character in input:  '%s' (ASCII=1) state=0 in %s on line %d

Warning: Unexpected character in input:  '%s' (ASCII=2) state=0 in %s on line %d

Warning: Unexpected character in input:  '%s' (ASCII=3) state=0 in %s on line %d
T_OPEN_TAG 6
T_BAD_CHARACTER 1
T_BAD_CHARACTER 1
T_WHITESPACE 1
T_STRING 3
T_WHITESPACE 1
T_BAD_CHARACTER 1
