--TEST--
basename() with various inputs
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip non-windows only test');
}
?>
--FILE--
<?php

$prefixes = array (
	
    // drive letters
	"A:/",
    "Z:/",
    "A:\\",

    // other prefixes
    "http://",
    "blah://",
	"blah:\\",
    "hostname:",

	// home directory ~
	"~/",
	"~\\",
);

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

foreach ($prefixes as $prefix) {
	foreach ($paths as $path) {
		$input = $prefix . $path;
		echo "basename for path $input is:\n";
		var_dump(basename($input));
	}
}

echo "\ndone\n";

?>
--EXPECT--
basename for path A:/foo is:
unicode(3) "foo"
basename for path A:/foo/ is:
unicode(3) "foo"
basename for path A:/foo\ is:
unicode(4) "foo\"
basename for path A:/foo.bar is:
unicode(7) "foo.bar"
basename for path A:/foo.bar/ is:
unicode(7) "foo.bar"
basename for path A:/foo.bar\ is:
unicode(8) "foo.bar\"
basename for path A:/dir/foo.bar is:
unicode(7) "foo.bar"
basename for path A:/dir\foo.bar is:
unicode(11) "dir\foo.bar"
basename for path A:/dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path A:/dir with spaces\foo.bar is:
unicode(23) "dir with spaces\foo.bar"
basename for path Z:/foo is:
unicode(3) "foo"
basename for path Z:/foo/ is:
unicode(3) "foo"
basename for path Z:/foo\ is:
unicode(4) "foo\"
basename for path Z:/foo.bar is:
unicode(7) "foo.bar"
basename for path Z:/foo.bar/ is:
unicode(7) "foo.bar"
basename for path Z:/foo.bar\ is:
unicode(8) "foo.bar\"
basename for path Z:/dir/foo.bar is:
unicode(7) "foo.bar"
basename for path Z:/dir\foo.bar is:
unicode(11) "dir\foo.bar"
basename for path Z:/dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path Z:/dir with spaces\foo.bar is:
unicode(23) "dir with spaces\foo.bar"
basename for path A:\foo is:
unicode(6) "A:\foo"
basename for path A:\foo/ is:
unicode(6) "A:\foo"
basename for path A:\foo\ is:
unicode(7) "A:\foo\"
basename for path A:\foo.bar is:
unicode(10) "A:\foo.bar"
basename for path A:\foo.bar/ is:
unicode(10) "A:\foo.bar"
basename for path A:\foo.bar\ is:
unicode(11) "A:\foo.bar\"
basename for path A:\dir/foo.bar is:
unicode(7) "foo.bar"
basename for path A:\dir\foo.bar is:
unicode(14) "A:\dir\foo.bar"
basename for path A:\dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path A:\dir with spaces\foo.bar is:
unicode(26) "A:\dir with spaces\foo.bar"
basename for path http://foo is:
unicode(3) "foo"
basename for path http://foo/ is:
unicode(3) "foo"
basename for path http://foo\ is:
unicode(4) "foo\"
basename for path http://foo.bar is:
unicode(7) "foo.bar"
basename for path http://foo.bar/ is:
unicode(7) "foo.bar"
basename for path http://foo.bar\ is:
unicode(8) "foo.bar\"
basename for path http://dir/foo.bar is:
unicode(7) "foo.bar"
basename for path http://dir\foo.bar is:
unicode(11) "dir\foo.bar"
basename for path http://dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path http://dir with spaces\foo.bar is:
unicode(23) "dir with spaces\foo.bar"
basename for path blah://foo is:
unicode(3) "foo"
basename for path blah://foo/ is:
unicode(3) "foo"
basename for path blah://foo\ is:
unicode(4) "foo\"
basename for path blah://foo.bar is:
unicode(7) "foo.bar"
basename for path blah://foo.bar/ is:
unicode(7) "foo.bar"
basename for path blah://foo.bar\ is:
unicode(8) "foo.bar\"
basename for path blah://dir/foo.bar is:
unicode(7) "foo.bar"
basename for path blah://dir\foo.bar is:
unicode(11) "dir\foo.bar"
basename for path blah://dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path blah://dir with spaces\foo.bar is:
unicode(23) "dir with spaces\foo.bar"
basename for path blah:\foo is:
unicode(9) "blah:\foo"
basename for path blah:\foo/ is:
unicode(9) "blah:\foo"
basename for path blah:\foo\ is:
unicode(10) "blah:\foo\"
basename for path blah:\foo.bar is:
unicode(13) "blah:\foo.bar"
basename for path blah:\foo.bar/ is:
unicode(13) "blah:\foo.bar"
basename for path blah:\foo.bar\ is:
unicode(14) "blah:\foo.bar\"
basename for path blah:\dir/foo.bar is:
unicode(7) "foo.bar"
basename for path blah:\dir\foo.bar is:
unicode(17) "blah:\dir\foo.bar"
basename for path blah:\dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path blah:\dir with spaces\foo.bar is:
unicode(29) "blah:\dir with spaces\foo.bar"
basename for path hostname:foo is:
unicode(12) "hostname:foo"
basename for path hostname:foo/ is:
unicode(12) "hostname:foo"
basename for path hostname:foo\ is:
unicode(13) "hostname:foo\"
basename for path hostname:foo.bar is:
unicode(16) "hostname:foo.bar"
basename for path hostname:foo.bar/ is:
unicode(16) "hostname:foo.bar"
basename for path hostname:foo.bar\ is:
unicode(17) "hostname:foo.bar\"
basename for path hostname:dir/foo.bar is:
unicode(7) "foo.bar"
basename for path hostname:dir\foo.bar is:
unicode(20) "hostname:dir\foo.bar"
basename for path hostname:dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path hostname:dir with spaces\foo.bar is:
unicode(32) "hostname:dir with spaces\foo.bar"
basename for path ~/foo is:
unicode(3) "foo"
basename for path ~/foo/ is:
unicode(3) "foo"
basename for path ~/foo\ is:
unicode(4) "foo\"
basename for path ~/foo.bar is:
unicode(7) "foo.bar"
basename for path ~/foo.bar/ is:
unicode(7) "foo.bar"
basename for path ~/foo.bar\ is:
unicode(8) "foo.bar\"
basename for path ~/dir/foo.bar is:
unicode(7) "foo.bar"
basename for path ~/dir\foo.bar is:
unicode(11) "dir\foo.bar"
basename for path ~/dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path ~/dir with spaces\foo.bar is:
unicode(23) "dir with spaces\foo.bar"
basename for path ~\foo is:
unicode(5) "~\foo"
basename for path ~\foo/ is:
unicode(5) "~\foo"
basename for path ~\foo\ is:
unicode(6) "~\foo\"
basename for path ~\foo.bar is:
unicode(9) "~\foo.bar"
basename for path ~\foo.bar/ is:
unicode(9) "~\foo.bar"
basename for path ~\foo.bar\ is:
unicode(10) "~\foo.bar\"
basename for path ~\dir/foo.bar is:
unicode(7) "foo.bar"
basename for path ~\dir\foo.bar is:
unicode(13) "~\dir\foo.bar"
basename for path ~\dir with spaces/foo.bar is:
unicode(7) "foo.bar"
basename for path ~\dir with spaces\foo.bar is:
unicode(25) "~\dir with spaces\foo.bar"

done