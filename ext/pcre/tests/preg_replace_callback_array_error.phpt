--TEST--
preg_replace_callback_array() errors
--FILE--
<?php

function b() {
    return "b";
}

// empty strings

var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        "" => function () { return "ok"; }), 'a'));

var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        null => function () { return "ok"; }), 'a'));

// backslashes

var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        "\\b\\" => function () { return "ok"; }), 'a'));

// alphanumeric delimiters

var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        "aba" => function () { return "ok"; }), 'a'));

var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        "1b1" => function () { return "ok"; }), 'a'));

// null character delimiter

var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',
        "\0b\0" => function () { return "ok"; }), 'a'));

?>
--EXPECTF--
Warning: preg_replace_callback_array(): Empty regular expression in %spreg_replace_callback_array_error.php on line %d
NULL

Warning: preg_replace_callback_array(): Empty regular expression in %spreg_replace_callback_array_error.php on line %d
NULL

Warning: preg_replace_callback_array(): Delimiter must not be alphanumeric, backslash, or NUL in %spreg_replace_callback_array_error.php on line %d
NULL

Warning: preg_replace_callback_array(): Delimiter must not be alphanumeric, backslash, or NUL in %spreg_replace_callback_array_error.php on line %d
NULL

Warning: preg_replace_callback_array(): Delimiter must not be alphanumeric, backslash, or NUL in %spreg_replace_callback_array_error.php on line %d
NULL

Warning: preg_replace_callback_array(): Delimiter must not be alphanumeric, backslash, or NUL in %spreg_replace_callback_array_error.php on line %d
NULL