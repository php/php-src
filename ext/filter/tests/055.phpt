--TEST--
filter_var() and FILTER_VALIDATE_MAC
--EXTENSIONS--
filter
--FILE--
<?php
$values = Array(
    array("01-23-45-67-89-ab", 0),
    array("01-23-45-67-89-ab", array("options" => array("separator" => "-"))),
    array("01-23-45-67-89-ab", array("options" => array("separator" => "."))),
    array("01-23-45-67-89-ab", array("options" => array("separator" => ":"))),
    array("01-23-45-67-89-AB", 0),
    array("01-23-45-67-89-aB", 0),
    array("01:23:45:67:89:ab", 0),
    array("01:23:45:67:89:AB", 0),
    array("01:23:45:67:89:aB", 0),
    array("01:23:45-67:89:aB", 0),
    array("xx:23:45:67:89:aB", 0),
    array("0123.4567.89ab", 0),
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
