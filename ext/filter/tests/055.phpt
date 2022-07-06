--TEST--
filter_var() and FILTER_VALIDATE_MAC
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$values = Array(
    array("01-23-45-67-89-ab", null),
    array("01-23-45-67-89-ab", array("options" => array("separator" => "-"))),
    array("01-23-45-67-89-ab", array("options" => array("separator" => "."))),
    array("01-23-45-67-89-ab", array("options" => array("separator" => ":"))),
    array("01-23-45-67-89-AB", null),
    array("01-23-45-67-89-aB", null),
    array("01:23:45:67:89:ab", null),
    array("01:23:45:67:89:AB", null),
    array("01:23:45:67:89:aB", null),
    array("01:23:45-67:89:aB", null),
    array("xx:23:45:67:89:aB", null),
    array("0123.4567.89ab", null),
    array("01-23-45-67-89-ab", array("options" => array("separator" => "--"))),
    array("01-23-45-67-89-ab", array("options" => array("separator" => ""))),
);
foreach ($values as $value) {
    try {
        var_dump(filter_var($value[0], FILTER_VALIDATE_MAC, $value[1]));
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

echo "Done\n";
?>
--EXPECT--
string(17) "01-23-45-67-89-ab"
string(17) "01-23-45-67-89-ab"
bool(false)
bool(false)
string(17) "01-23-45-67-89-AB"
string(17) "01-23-45-67-89-aB"
string(17) "01:23:45:67:89:ab"
string(17) "01:23:45:67:89:AB"
string(17) "01:23:45:67:89:aB"
bool(false)
bool(false)
string(14) "0123.4567.89ab"
filter_var(): "separator" option must be one character long
filter_var(): "separator" option must be one character long
Done
