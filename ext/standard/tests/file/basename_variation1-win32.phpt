--TEST--
basename() with various inputs
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip Windows only basename tests');
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
string(3) "foo"
basename for path A:/foo/ is:
string(3) "foo"
basename for path A:/foo\ is:
string(3) "foo"
basename for path A:/foo.bar is:
string(7) "foo.bar"
basename for path A:/foo.bar/ is:
string(7) "foo.bar"
basename for path A:/foo.bar\ is:
string(7) "foo.bar"
basename for path A:/dir/foo.bar is:
string(7) "foo.bar"
basename for path A:/dir\foo.bar is:
string(7) "foo.bar"
basename for path A:/dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path A:/dir with spaces\foo.bar is:
string(7) "foo.bar"
basename for path Z:/foo is:
string(3) "foo"
basename for path Z:/foo/ is:
string(3) "foo"
basename for path Z:/foo\ is:
string(3) "foo"
basename for path Z:/foo.bar is:
string(7) "foo.bar"
basename for path Z:/foo.bar/ is:
string(7) "foo.bar"
basename for path Z:/foo.bar\ is:
string(7) "foo.bar"
basename for path Z:/dir/foo.bar is:
string(7) "foo.bar"
basename for path Z:/dir\foo.bar is:
string(7) "foo.bar"
basename for path Z:/dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path Z:/dir with spaces\foo.bar is:
string(7) "foo.bar"
basename for path A:\foo is:
string(3) "foo"
basename for path A:\foo/ is:
string(3) "foo"
basename for path A:\foo\ is:
string(3) "foo"
basename for path A:\foo.bar is:
string(7) "foo.bar"
basename for path A:\foo.bar/ is:
string(7) "foo.bar"
basename for path A:\foo.bar\ is:
string(7) "foo.bar"
basename for path A:\dir/foo.bar is:
string(7) "foo.bar"
basename for path A:\dir\foo.bar is:
string(7) "foo.bar"
basename for path A:\dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path A:\dir with spaces\foo.bar is:
string(7) "foo.bar"
basename for path http://foo is:
string(3) "foo"
basename for path http://foo/ is:
string(3) "foo"
basename for path http://foo\ is:
string(3) "foo"
basename for path http://foo.bar is:
string(7) "foo.bar"
basename for path http://foo.bar/ is:
string(7) "foo.bar"
basename for path http://foo.bar\ is:
string(7) "foo.bar"
basename for path http://dir/foo.bar is:
string(7) "foo.bar"
basename for path http://dir\foo.bar is:
string(7) "foo.bar"
basename for path http://dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path http://dir with spaces\foo.bar is:
string(7) "foo.bar"
basename for path blah://foo is:
string(3) "foo"
basename for path blah://foo/ is:
string(3) "foo"
basename for path blah://foo\ is:
string(3) "foo"
basename for path blah://foo.bar is:
string(7) "foo.bar"
basename for path blah://foo.bar/ is:
string(7) "foo.bar"
basename for path blah://foo.bar\ is:
string(7) "foo.bar"
basename for path blah://dir/foo.bar is:
string(7) "foo.bar"
basename for path blah://dir\foo.bar is:
string(7) "foo.bar"
basename for path blah://dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path blah://dir with spaces\foo.bar is:
string(7) "foo.bar"
basename for path blah:\foo is:
string(3) "foo"
basename for path blah:\foo/ is:
string(3) "foo"
basename for path blah:\foo\ is:
string(3) "foo"
basename for path blah:\foo.bar is:
string(7) "foo.bar"
basename for path blah:\foo.bar/ is:
string(7) "foo.bar"
basename for path blah:\foo.bar\ is:
string(7) "foo.bar"
basename for path blah:\dir/foo.bar is:
string(7) "foo.bar"
basename for path blah:\dir\foo.bar is:
string(7) "foo.bar"
basename for path blah:\dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path blah:\dir with spaces\foo.bar is:
string(7) "foo.bar"
basename for path hostname:foo is:
string(12) "hostname:foo"
basename for path hostname:foo/ is:
string(12) "hostname:foo"
basename for path hostname:foo\ is:
string(12) "hostname:foo"
basename for path hostname:foo.bar is:
string(16) "hostname:foo.bar"
basename for path hostname:foo.bar/ is:
string(16) "hostname:foo.bar"
basename for path hostname:foo.bar\ is:
string(16) "hostname:foo.bar"
basename for path hostname:dir/foo.bar is:
string(7) "foo.bar"
basename for path hostname:dir\foo.bar is:
string(7) "foo.bar"
basename for path hostname:dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path hostname:dir with spaces\foo.bar is:
string(7) "foo.bar"
basename for path ~/foo is:
string(3) "foo"
basename for path ~/foo/ is:
string(3) "foo"
basename for path ~/foo\ is:
string(3) "foo"
basename for path ~/foo.bar is:
string(7) "foo.bar"
basename for path ~/foo.bar/ is:
string(7) "foo.bar"
basename for path ~/foo.bar\ is:
string(7) "foo.bar"
basename for path ~/dir/foo.bar is:
string(7) "foo.bar"
basename for path ~/dir\foo.bar is:
string(7) "foo.bar"
basename for path ~/dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path ~/dir with spaces\foo.bar is:
string(7) "foo.bar"
basename for path ~\foo is:
string(3) "foo"
basename for path ~\foo/ is:
string(3) "foo"
basename for path ~\foo\ is:
string(3) "foo"
basename for path ~\foo.bar is:
string(7) "foo.bar"
basename for path ~\foo.bar/ is:
string(7) "foo.bar"
basename for path ~\foo.bar\ is:
string(7) "foo.bar"
basename for path ~\dir/foo.bar is:
string(7) "foo.bar"
basename for path ~\dir\foo.bar is:
string(7) "foo.bar"
basename for path ~\dir with spaces/foo.bar is:
string(7) "foo.bar"
basename for path ~\dir with spaces\foo.bar is:
string(7) "foo.bar"

done
