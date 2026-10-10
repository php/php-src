--TEST--
GH-23958: Truncated encoding names must not be accepted
--EXTENSIONS--
mbstring
--FILE--
<?php
foreach (['pa', 'UCS-', '',] as $name) {
    try {
        mb_http_output($name);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
try {
    mb_detect_order('aut');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(ini_set('mbstring.detect_order', ','));

try {
    mb_detect_encoding(bin2hex('1b24422422242424262428242a1b2842'), 'CP50220, ANSI_X3.4-1968,', false);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
mb_http_output(): Argument #1 ($encoding) must be a valid encoding, "pa" given
mb_http_output(): Argument #1 ($encoding) must be a valid encoding, "UCS-" given
mb_http_output(): Argument #1 ($encoding) must be a valid encoding, "" given
mb_detect_order(): Argument #1 ($encoding) contains invalid encoding "aut"

Warning: ini_set(): INI setting contains invalid encoding "" in %s on line %d
bool(false)
mb_detect_encoding(): Argument #2 ($encodings) contains invalid encoding ""
