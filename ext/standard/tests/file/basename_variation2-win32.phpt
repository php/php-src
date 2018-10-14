--TEST--
Testing basename() with various values for the suffix parameter
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip Windows only basename tests');
}
?>
--FILE--
<?php

$paths = array (

	"foo",
	"foo/",
    "foo\\",
    "foo.bar",
    "foo.bar/",
    "foo.bar\\",
    "dir/foo.bar",
    "dir\\foo.bar",
    "dir with spaces/foo.bar",
    "dir with spaces\\foo.bar",

);

$suffixes = array (

	".bar",
	".b",
    ".",
    " ",
    "foo",
    "foo.bar",
    "foo/bar",
    "foo\\bar",
    "/",
    "\\",
);

foreach ($paths as $path) {
	foreach ($suffixes as $suffix) {
		echo "basename for path $path, supplying suffix $suffix is:\n";
		var_dump(basename($path, $suffix));
	}
}

echo "\ndone\n";

?>
--EXPECT--
basename for path foo, supplying suffix .bar is:
string(3) "foo"
basename for path foo, supplying suffix .b is:
string(3) "foo"
basename for path foo, supplying suffix . is:
string(3) "foo"
basename for path foo, supplying suffix   is:
string(3) "foo"
basename for path foo, supplying suffix foo is:
string(3) "foo"
basename for path foo, supplying suffix foo.bar is:
string(3) "foo"
basename for path foo, supplying suffix foo/bar is:
string(3) "foo"
basename for path foo, supplying suffix foo\bar is:
string(3) "foo"
basename for path foo, supplying suffix / is:
string(3) "foo"
basename for path foo, supplying suffix \ is:
string(3) "foo"
basename for path foo/, supplying suffix .bar is:
string(3) "foo"
basename for path foo/, supplying suffix .b is:
string(3) "foo"
basename for path foo/, supplying suffix . is:
string(3) "foo"
basename for path foo/, supplying suffix   is:
string(3) "foo"
basename for path foo/, supplying suffix foo is:
string(3) "foo"
basename for path foo/, supplying suffix foo.bar is:
string(3) "foo"
basename for path foo/, supplying suffix foo/bar is:
string(3) "foo"
basename for path foo/, supplying suffix foo\bar is:
string(3) "foo"
basename for path foo/, supplying suffix / is:
string(3) "foo"
basename for path foo/, supplying suffix \ is:
string(3) "foo"
basename for path foo\, supplying suffix .bar is:
string(3) "foo"
basename for path foo\, supplying suffix .b is:
string(3) "foo"
basename for path foo\, supplying suffix . is:
string(3) "foo"
basename for path foo\, supplying suffix   is:
string(3) "foo"
basename for path foo\, supplying suffix foo is:
string(3) "foo"
basename for path foo\, supplying suffix foo.bar is:
string(3) "foo"
basename for path foo\, supplying suffix foo/bar is:
string(3) "foo"
basename for path foo\, supplying suffix foo\bar is:
string(3) "foo"
basename for path foo\, supplying suffix / is:
string(3) "foo"
basename for path foo\, supplying suffix \ is:
string(3) "foo"
basename for path foo.bar, supplying suffix .bar is:
string(3) "foo"
basename for path foo.bar, supplying suffix .b is:
string(7) "foo.bar"
basename for path foo.bar, supplying suffix . is:
string(7) "foo.bar"
basename for path foo.bar, supplying suffix   is:
string(7) "foo.bar"
basename for path foo.bar, supplying suffix foo is:
string(7) "foo.bar"
basename for path foo.bar, supplying suffix foo.bar is:
string(7) "foo.bar"
basename for path foo.bar, supplying suffix foo/bar is:
string(7) "foo.bar"
basename for path foo.bar, supplying suffix foo\bar is:
string(7) "foo.bar"
basename for path foo.bar, supplying suffix / is:
string(7) "foo.bar"
basename for path foo.bar, supplying suffix \ is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix .bar is:
string(3) "foo"
basename for path foo.bar/, supplying suffix .b is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix . is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix   is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix foo is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix foo.bar is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix foo/bar is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix foo\bar is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix / is:
string(7) "foo.bar"
basename for path foo.bar/, supplying suffix \ is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix .bar is:
string(3) "foo"
basename for path foo.bar\, supplying suffix .b is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix . is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix   is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix foo is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix foo.bar is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix foo/bar is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix foo\bar is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix / is:
string(7) "foo.bar"
basename for path foo.bar\, supplying suffix \ is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix .bar is:
string(3) "foo"
basename for path dir/foo.bar, supplying suffix .b is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix . is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix   is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix foo is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix foo.bar is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix foo/bar is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix foo\bar is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix / is:
string(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix \ is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix .bar is:
string(3) "foo"
basename for path dir\foo.bar, supplying suffix .b is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix . is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix   is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix foo is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix foo.bar is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix foo/bar is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix foo\bar is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix / is:
string(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix \ is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix .bar is:
string(3) "foo"
basename for path dir with spaces/foo.bar, supplying suffix .b is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix . is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix   is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix foo is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix foo.bar is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix foo/bar is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix foo\bar is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix / is:
string(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix \ is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix .bar is:
string(3) "foo"
basename for path dir with spaces\foo.bar, supplying suffix .b is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix . is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix   is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix foo is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix foo.bar is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix foo/bar is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix foo\bar is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix / is:
string(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix \ is:
string(7) "foo.bar"

done
