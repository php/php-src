--TEST--
more fgetss() tests
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

$filename = __DIR__."/fgetss1.html";

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
	var_dump(fgetss($fp, 1000, "<i>,<b>,<a>"));
	var_dump(fgetss($fp));
}

foreach ($array as $str) {
	file_put_contents($filename, $str);
	$fp = fopen($filename, "r");
	var_dump(fgetss($fp, 10));
	var_dump(fgetss($fp, 10, "<script>,<a>"));
}

echo "Done\n";
?>
--CLEAN--
<?php
$filename = __DIR__."/fgetss1.html";
unlink($filename);
?>
--EXPECT--
string(21) "askasdfasdf<b>aaaaaa
"
string(6) "dddddd"
string(28) "asdqw<i onClick="hello();">
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
string(9) "t>functio"
string(6) "asdasd"
string(0) ""
bool(false)
bool(false)
string(9) "some text"
string(2) " 
"
string(9) "some anot"
string(9) "her text "
Done
