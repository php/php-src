--TEST--
filter_var() and FILTER_VALIDATE_MAC
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$values = Array(
	"01-23-45-67-89-ab",
	"01-23-45-67-89-AB",
	"01-23-45-67-89-aB",
	"01:23:45:67:89:ab",
	"01:23:45:67:89:AB",
	"01:23:45:67:89:aB",
	"01:23:45-67:89:aB",
	"xx:23:45:67:89:aB",
	"0123.4567.89ab",
);
foreach ($values as $value) {
	var_dump(filter_var($value, FILTER_VALIDATE_MAC));
}

echo "Done\n";
?>
--EXPECT--	
string(17) "01-23-45-67-89-ab"
string(17) "01-23-45-67-89-AB"
string(17) "01-23-45-67-89-aB"
string(17) "01:23:45:67:89:ab"
string(17) "01:23:45:67:89:AB"
string(17) "01:23:45:67:89:aB"
bool(false)
bool(false)
string(14) "0123.4567.89ab"
Done
