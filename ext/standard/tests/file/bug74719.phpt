--TEST--
Bug #74719 Allow NULL as context, testing fopen, rename, unlink, mkdir and rmdir
--CREDITS--
Alexander Holman <alexander@holman.org.uk>
--FILE--
<?php
/* Prototype  : resource fopen(string filename, string mode [, bool use_include_path [, resource context]])
 * Description: Open a file or a URL and return a file pointer 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

require_once('fopen_include_path.inc');

$thisTestDir =  basename(__FILE__, ".php") . ".dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$newpath = relative_include_path();
set_include_path($newpath);

// fopen
$tmpfile =  basename(__FILE__, ".php") . ".tmp";
$h = fopen($tmpfile, "w", true, NULL);
if ($h !== false) {
    echo "ok\n";
    fclose($h);
}
$newTmpFile = "renamed_" . $tmpfile;

if (rename($tmpfile, $newTmpFile, NULL)) {
    echo "ok\n";
}

if (unlink($newTmpFile, NULL)) {
    echo "ok\n";
}

$tmpDir = "tmpDir";

if (mkdir($tmpDir, 0777, false, NULL)) {
    echo "ok\n";
}

if (rmdir($tmpDir, NULL)) {
    echo "ok\n";
}

teardown_relative_path();
restore_include_path();
chdir("..");
rmdir($thisTestDir);
?>
===DONE===
--EXPECT--
ok
ok
ok
ok
ok
===DONE===
