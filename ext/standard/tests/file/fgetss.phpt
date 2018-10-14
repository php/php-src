--TEST--
fgetss() tests
--FILE--
<?php

$filename = dirname(__FILE__)."/fgetss.html";

$array = array(
	"askasdfasdf<b>aaaaaa\n</b>dddddd",
	"asdqw<i onClick=\"hello();\">\naaaa<>qqqq",
	"aaa<script>function foo() {}</script>qqq",
	"asdasd<a\n asdjeje",
	"",
	"some text \n<b>blah</i>",
	"some another text <> hoho </>"
	);

foreach ($array as $str) {
	file_put_contents($filename, $str);
	$fp = fopen($filename, "r");
	var_dump(fgetss($fp));
	var_dump(fgetss($fp));
}

foreach ($array as $str) {
	file_put_contents($filename, $str);
	$fp = fopen($filename, "r");
	var_dump(fgetss($fp, 10));
	var_dump(fgetss($fp, 10));
}

var_dump(fgetss($fp, -10));
var_dump(fgetss($fp, 0));
fclose($fp);
var_dump(fgetss($fp, 0));

@unlink($filename);

echo "Done\n";
?>
--EXPECTF--
Deprecated: Function fgetss() is deprecated in %s on line %d
string(18) "askasdfasdfaaaaaa
"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(6) "dddddd"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(6) "asdqw
"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(8) "aaaaqqqq"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(23) "aaafunction foo() {}qqq"

Deprecated: Function fgetss() is deprecated in %s on line %d
bool(false)

Deprecated: Function fgetss() is deprecated in %s on line %d
string(6) "asdasd"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(0) ""

Deprecated: Function fgetss() is deprecated in %s on line %d
bool(false)

Deprecated: Function fgetss() is deprecated in %s on line %d
bool(false)

Deprecated: Function fgetss() is deprecated in %s on line %d
string(11) "some text 
"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(4) "blah"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(24) "some another text  hoho "

Deprecated: Function fgetss() is deprecated in %s on line %d
bool(false)

Deprecated: Function fgetss() is deprecated in %s on line %d
string(9) "askasdfas"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(6) "dfaaaa"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(5) "asdqw"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(0) ""

Deprecated: Function fgetss() is deprecated in %s on line %d
string(3) "aaa"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(7) "functio"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(6) "asdasd"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(0) ""

Deprecated: Function fgetss() is deprecated in %s on line %d
bool(false)

Deprecated: Function fgetss() is deprecated in %s on line %d
bool(false)

Deprecated: Function fgetss() is deprecated in %s on line %d
string(9) "some text"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(2) " 
"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(9) "some anot"

Deprecated: Function fgetss() is deprecated in %s on line %d
string(9) "her text "

Deprecated: Function fgetss() is deprecated in %s on line %d

Warning: fgetss(): Length parameter must be greater than 0 in %s on line %d
bool(false)

Deprecated: Function fgetss() is deprecated in %s on line %d

Warning: fgetss(): Length parameter must be greater than 0 in %s on line %d
bool(false)

Deprecated: Function fgetss() is deprecated in %s on line %d

Warning: fgetss(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
Done
