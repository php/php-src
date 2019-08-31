--TEST--
Bug #75063 Many filesystem-related functions do not work with multibyte file names, UTF-8
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php

/* This file is in UTF-8. */

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$dir_basename = "тест";
$prefix = __DIR__ . DIRECTORY_SEPARATOR . "bug75063-utf8";
$d0 = $prefix . DIRECTORY_SEPARATOR . $dir_basename;

mkdir($prefix);
create_verify_dir($prefix, $dir_basename);

var_dump(get_basename_with_cp($d0, 65001, false));

$old_cwd = getcwd();
var_dump(chdir($d0));

$code = <<<CODE
<?php

foreach(["test", "таст"] as \$fn) {
	file_put_contents("\$fn.txt", "");
}

var_dump(getcwd());
if (\$dh = opendir(getcwd())) {
	while ((\$file = readdir(\$dh)) !== false) {
		if ("." == \$file || ".." == \$file) continue;
		var_dump(\$file);
	}
	closedir(\$dh);
}
CODE;
$code_fn = "code.php";
file_put_contents($code_fn, $code);

print(shell_exec(getenv('TEST_PHP_EXECUTABLE') . " -nf code.php"));

chdir($old_cwd);

?>
===DONE===
--CLEAN--
<?php
$dir_basename = "тест";
$prefix = __DIR__ . DIRECTORY_SEPARATOR . "bug75063-utf8";
$d0 = $prefix . DIRECTORY_SEPARATOR . $dir_basename;

$obj = scandir($d0);
foreach ($obj as $file) {
	if ("." == $file || ".." == $file) continue;
	unlink($d0 . DIRECTORY_SEPARATOR . $file);
}

rmdir($d0);
rmdir($prefix);

?>
--EXPECTF--
string(8) "тест"
bool(true)
string(%d) "%sbug75063-utf8%eтест"
string(8) "code.php"
string(8) "test.txt"
string(12) "таст.txt"
===DONE===
