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
string(18) "askasdfasdfaaaaaa
"
string(6) "dddddd"
string(6) "asdqw
"
string(8) "aaaaqqqq"
string(23) "aaafunction foo() {}qqq"
bool(false)
string(6) "asdasd"
string(0) ""
bool(false)
bool(false)
string(11) "some text 
"
string(4) "blah"
string(24) "some another text  hoho "
bool(false)
string(9) "askasdfas"
string(6) "dfaaaa"
string(5) "asdqw"
string(0) ""
string(3) "aaa"
string(7) "functio"
string(6) "asdasd"
string(0) ""
bool(false)
bool(false)
string(9) "some text"
string(2) " 
"
string(9) "some anot"
string(9) "her text "

Warning: fgetss(): Length parameter must be greater than 0 in %s on line %d
bool(false)

Warning: fgetss(): Length parameter must be greater than 0 in %s on line %d
bool(false)

Warning: fgetss(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
Done
