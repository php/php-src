--TEST--
Thai UTF-8 basic test
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php
/*
#vim: set fileencoding=cp874
#vim: set encoding=cp874
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$names = array( /* cp874 */
	"�������跴�ͺ1",
	"�������跴�ͺ2",
	"�������跴�ͺ3",
	"�������跴�ͺ4",
	"�������跴�ͺ5",
	"�������跴�ͺ6",
	"�������跴�ͺ7",
	"�������跴�ͺ8",
	"�������跴�ͺ8 10",
);

$i = 0;
foreach ($names as $name) {
	$pathw = dirname(__FILE__) . DIRECTORY_SEPARATOR . iconv('cp874', 'utf-8', $name) . ".txt";

	file_put_contents($pathw, "hello" . $i++);

	get_basename_with_cp($pathw, 65001);
	var_dump(file_get_contents($pathw));

	unlink($pathw);
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
