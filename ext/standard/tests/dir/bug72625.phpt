--TEST--
Bug #72625 realpath() fails on very long argument.
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php

$base = sys_get_temp_dir() . "/" . md5(uniqid());
while (strlen($base) < 260) {
	$base = "$base/" . md5(uniqid());
}

$f0 = "$base/_test/documents/projects/myproject/vendor/name/library/classpath";
$f1 = "$f0/../../../../../../../../_test/documents/projects/myproject/vendor/name/library/../../../../../../../_test/documents/projects/myproject/vendor/name/library/classpath";


mkdir($f0, 0777, true);


var_dump(
	$f0,
	file_exists($f0),
	realpath($f0),
	dirname($f0),

	$f1,
	file_exists($f1),
	realpath($f1),
	dirname($f1)
);

$tmp = $f0;
while ($tmp > $base) {
	rmdir($tmp);
	$tmp = dirname($tmp);
}

?>
===DONE===
--EXPECTF--
string(%d) "%s/_test/documents/projects/myproject/vendor/name/library/classpath"
bool(true)
string(%d) "%s\_test\documents\projects\myproject\vendor\name\library\classpath"
string(%d) "%s/_test/documents/projects/myproject/vendor/name/library"
string(%d) "%s/_test/documents/projects/myproject/vendor/name/library/classpath/../../../../../../../../_test/documents/projects/myproject/vendor/name/library/../../../../../../../_test/documents/projects/myproject/vendor/name/library/classpath"
bool(true)
string(%d) "%s\_test\documents\projects\myproject\vendor\name\library\classpath"
string(%d) "%s/_test/documents/projects/myproject/vendor/name/library"
===DONE===
