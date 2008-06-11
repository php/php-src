--TEST--
Testing basename() with various values for the suffix parameter
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip non-windows only test');
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
unicode(3) "foo"
basename for path foo, supplying suffix .b is:
unicode(3) "foo"
basename for path foo, supplying suffix . is:
unicode(3) "foo"
basename for path foo, supplying suffix   is:
unicode(3) "foo"
basename for path foo, supplying suffix foo is:
unicode(3) "foo"
basename for path foo, supplying suffix foo.bar is:
unicode(3) "foo"
basename for path foo, supplying suffix foo/bar is:
unicode(3) "foo"
basename for path foo, supplying suffix foo\bar is:
unicode(3) "foo"
basename for path foo, supplying suffix / is:
unicode(3) "foo"
basename for path foo, supplying suffix \ is:
unicode(3) "foo"
basename for path foo/, supplying suffix .bar is:
unicode(3) "foo"
basename for path foo/, supplying suffix .b is:
unicode(3) "foo"
basename for path foo/, supplying suffix . is:
unicode(3) "foo"
basename for path foo/, supplying suffix   is:
unicode(3) "foo"
basename for path foo/, supplying suffix foo is:
unicode(3) "foo"
basename for path foo/, supplying suffix foo.bar is:
unicode(3) "foo"
basename for path foo/, supplying suffix foo/bar is:
unicode(3) "foo"
basename for path foo/, supplying suffix foo\bar is:
unicode(3) "foo"
basename for path foo/, supplying suffix / is:
unicode(3) "foo"
basename for path foo/, supplying suffix \ is:
unicode(3) "foo"
basename for path foo\, supplying suffix .bar is:
unicode(4) "foo\"
basename for path foo\, supplying suffix .b is:
unicode(4) "foo\"
basename for path foo\, supplying suffix . is:
unicode(4) "foo\"
basename for path foo\, supplying suffix   is:
unicode(4) "foo\"
basename for path foo\, supplying suffix foo is:
unicode(4) "foo\"
basename for path foo\, supplying suffix foo.bar is:
unicode(4) "foo\"
basename for path foo\, supplying suffix foo/bar is:
unicode(4) "foo\"
basename for path foo\, supplying suffix foo\bar is:
unicode(4) "foo\"
basename for path foo\, supplying suffix / is:
unicode(4) "foo\"
basename for path foo\, supplying suffix \ is:
unicode(3) "foo"
basename for path foo.bar, supplying suffix .bar is:
unicode(3) "foo"
basename for path foo.bar, supplying suffix .b is:
unicode(7) "foo.bar"
basename for path foo.bar, supplying suffix . is:
unicode(7) "foo.bar"
basename for path foo.bar, supplying suffix   is:
unicode(7) "foo.bar"
basename for path foo.bar, supplying suffix foo is:
unicode(7) "foo.bar"
basename for path foo.bar, supplying suffix foo.bar is:
unicode(7) "foo.bar"
basename for path foo.bar, supplying suffix foo/bar is:
unicode(7) "foo.bar"
basename for path foo.bar, supplying suffix foo\bar is:
unicode(7) "foo.bar"
basename for path foo.bar, supplying suffix / is:
unicode(7) "foo.bar"
basename for path foo.bar, supplying suffix \ is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix .bar is:
unicode(3) "foo"
basename for path foo.bar/, supplying suffix .b is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix . is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix   is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix foo is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix foo.bar is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix foo/bar is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix foo\bar is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix / is:
unicode(7) "foo.bar"
basename for path foo.bar/, supplying suffix \ is:
unicode(7) "foo.bar"
basename for path foo.bar\, supplying suffix .bar is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix .b is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix . is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix   is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix foo is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix foo.bar is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix foo/bar is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix foo\bar is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix / is:
unicode(8) "foo.bar\"
basename for path foo.bar\, supplying suffix \ is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix .bar is:
unicode(3) "foo"
basename for path dir/foo.bar, supplying suffix .b is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix . is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix   is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix foo is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix foo.bar is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix foo/bar is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix foo\bar is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix / is:
unicode(7) "foo.bar"
basename for path dir/foo.bar, supplying suffix \ is:
unicode(7) "foo.bar"
basename for path dir\foo.bar, supplying suffix .bar is:
unicode(7) "dir\foo"
basename for path dir\foo.bar, supplying suffix .b is:
unicode(11) "dir\foo.bar"
basename for path dir\foo.bar, supplying suffix . is:
unicode(11) "dir\foo.bar"
basename for path dir\foo.bar, supplying suffix   is:
unicode(11) "dir\foo.bar"
basename for path dir\foo.bar, supplying suffix foo is:
unicode(11) "dir\foo.bar"
basename for path dir\foo.bar, supplying suffix foo.bar is:
unicode(4) "dir\"
basename for path dir\foo.bar, supplying suffix foo/bar is:
unicode(11) "dir\foo.bar"
basename for path dir\foo.bar, supplying suffix foo\bar is:
unicode(11) "dir\foo.bar"
basename for path dir\foo.bar, supplying suffix / is:
unicode(11) "dir\foo.bar"
basename for path dir\foo.bar, supplying suffix \ is:
unicode(11) "dir\foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix .bar is:
unicode(3) "foo"
basename for path dir with spaces/foo.bar, supplying suffix .b is:
unicode(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix . is:
unicode(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix   is:
unicode(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix foo is:
unicode(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix foo.bar is:
unicode(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix foo/bar is:
unicode(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix foo\bar is:
unicode(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix / is:
unicode(7) "foo.bar"
basename for path dir with spaces/foo.bar, supplying suffix \ is:
unicode(7) "foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix .bar is:
unicode(19) "dir with spaces\foo"
basename for path dir with spaces\foo.bar, supplying suffix .b is:
unicode(23) "dir with spaces\foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix . is:
unicode(23) "dir with spaces\foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix   is:
unicode(23) "dir with spaces\foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix foo is:
unicode(23) "dir with spaces\foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix foo.bar is:
unicode(16) "dir with spaces\"
basename for path dir with spaces\foo.bar, supplying suffix foo/bar is:
unicode(23) "dir with spaces\foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix foo\bar is:
unicode(23) "dir with spaces\foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix / is:
unicode(23) "dir with spaces\foo.bar"
basename for path dir with spaces\foo.bar, supplying suffix \ is:
unicode(23) "dir with spaces\foo.bar"

done
