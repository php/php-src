--TEST--
Bug #28790: Add php.ini option to disable stat cache (without cache)
--INI--
enable_stat_cache = False
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

# This deletes the file and shouldn't emit stat or lstat messages.
passthru($php.' -n -r "unlink(\\"'.$qtestfile.'\\");"');
all_the_stats("$testfile", "testfile exists (it shouldn't)");

# This stats a non-existent file; still no testfile stat or lstat messages.
if (!@stat("$impossiblefile")) {
    print("stat impossiblefile does not exist.\n");
}
all_the_stats("$testfile", "testfile exists (it shouldn't)");

# This is_files an existing file; still no testfile stat or lstat messages.
if (is_file("$phpfile")) {
    print("is_file(stat): php binary exists.\n");
}
all_the_stats("$testfile", "testfile exists (it shouldn't)");

# This lstats an existing file; still no testfile stat or lstat messages.
if (lstat("$phpfile")) {
    print("lstat: php binary exists.\n");
}
all_the_stats("$testfile", "testfile exists (it shouldn't)");

?>
--EXPECT--
lstat: testfile exists.
stat: testfile exists.
stat impossiblefile does not exist.
is_file(stat): php binary exists.
lstat: php binary exists.
