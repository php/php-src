--TEST--
Bug #28790: Add php.ini option to disable stat cache (with cache)
--FILE--
<?php

$php = '"'.getenv('TEST_PHP_EXECUTABLE').'"';
$phpfile = getenv('TEST_PHP_EXECUTABLE');
$impossiblefile = __FILE__.DIRECTORY_SEPARATOR.'bug28790.impossible';
$testfile = __DIR__.DIRECTORY_SEPARATOR.'bug28790.file';

function all_the_stats($filename, $message) {
    if (@lstat($filename)) {
        print("lstat: $message.\n");
    }
    if (@stat($filename)) {
        print("stat: $message.\n");
    }
}

# Windows can use / for dir separators, so let's do that.
$qtestfile = str_replace("\\", "/", "$testfile");

# This creates a file and should emit stat & lstat messages.
passthru($php.' -n -r "touch(\\"'.$qtestfile.'\\");"');
all_the_stats("$testfile", "testfile exists");

# This deletes the file and shouldn't emit stat & lstat messages (but does).
passthru($php.' -n -r "unlink(\\"'.$qtestfile.'\\");"');
all_the_stats("$testfile", "testfile exists (it shouldn't)");

# This stats a non-existent file; still stat/lstats the deleted testfile (shouldn't).
if (!@stat("$impossiblefile")) {
    print("stat impossiblefile does not exist.\n");
}
all_the_stats("$testfile", "testfile exists (it shouldn't)");

# This is_files an existing file; still can lstat the deleted testfile (shouldn't).
if (is_file("$phpfile")) {
    print("is_file(stat): php binary exists.\n");
}
all_the_stats("$testfile", "testfile exists (it shouldn't)");

# This lstats an existing file; finally can't stat or lstat the deleted testfile.
if (lstat("$phpfile")) {
    print("lstat: php binary exists.\n");
}
all_the_stats("$testfile", "testfile exists (it shouldn't)");

?>
--EXPECT--
lstat: testfile exists.
stat: testfile exists.
lstat: testfile exists (it shouldn't).
stat: testfile exists (it shouldn't).
stat impossiblefile does not exist.
lstat: testfile exists (it shouldn't).
stat: testfile exists (it shouldn't).
is_file(stat): php binary exists.
lstat: testfile exists (it shouldn't).
lstat: php binary exists.
