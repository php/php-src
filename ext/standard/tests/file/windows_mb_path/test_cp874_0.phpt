--TEST--
Thai cp874 basic test
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();
skip_if_wrong_cp(874, "oem");

?>
--INI--
default_charset=cp874
--FILE--
<?php
/*
#vim: set fileencoding=cp874
#vim: set encoding=cp874
*/

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$names = array( /* cp874 */
	"เป็นแฟ้มที่ทดสอบ1",
	"เป็นแฟ้มที่ทดสอบ2",
	"เป็นแฟ้มที่ทดสอบ3",
	"เป็นแฟ้มที่ทดสอบ4",
	"เป็นแฟ้มที่ทดสอบ5",
	"เป็นแฟ้มที่ทดสอบ6",
	"เป็นแฟ้มที่ทดสอบ7",
	"เป็นแฟ้มที่ทดสอบ8",
	"เป็นแฟ้มที่ทดสอบ8 10",
);

$i = 0;
foreach ($names as $name) {
	$path = __DIR__ . DIRECTORY_SEPARATOR . $name . ".txt";

	file_put_contents($path, "hello" . $i++);

	get_basename_with_cp($path, 874);
	var_dump(file_get_contents($path));

	unlink($path);
}

?>
===DONE===
--EXPECTF--
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ1.txt
string(%d) "เป็นแฟ้มที่ทดสอบ1.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ1.txt"
Active code page: %d
string(6) "hello0"
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ2.txt
string(%d) "เป็นแฟ้มที่ทดสอบ2.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ2.txt"
Active code page: %d
string(6) "hello1"
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ3.txt
string(%d) "เป็นแฟ้มที่ทดสอบ3.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ3.txt"
Active code page: %d
string(6) "hello2"
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ4.txt
string(%d) "เป็นแฟ้มที่ทดสอบ4.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ4.txt"
Active code page: %d
string(6) "hello3"
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ5.txt
string(%d) "เป็นแฟ้มที่ทดสอบ5.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ5.txt"
Active code page: %d
string(6) "hello4"
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ6.txt
string(%d) "เป็นแฟ้มที่ทดสอบ6.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ6.txt"
Active code page: %d
string(6) "hello5"
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ7.txt
string(%d) "เป็นแฟ้มที่ทดสอบ7.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ7.txt"
Active code page: %d
string(6) "hello6"
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ8.txt
string(%d) "เป็นแฟ้มที่ทดสอบ8.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ8.txt"
Active code page: %d
string(6) "hello7"
Active code page: %d
getting basename of %sเป็นแฟ้มที่ทดสอบ8 10.txt
string(%d) "เป็นแฟ้มที่ทดสอบ8 10.txt"
bool(true)
string(%d) "%sเป็นแฟ้มที่ทดสอบ8 10.txt"
Active code page: %d
string(6) "hello8"
===DONE===
