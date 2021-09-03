--TEST--
Bug #28790: Add php.ini option to disable stat cache (with cache)
--FILE--
<?php

$php = '"'.getenv('TEST_PHP_EXECUTABLE').'"';
$phpfile = getenv('TEST_PHP_EXECUTABLE');
$impossiblefile = $phpfile.DIRECTORY_SEPARATOR.'bug28790.impossible';
$testfile = __DIR__.DIRECTORY_SEPARATOR.'bug28790.file';

function all_the_stats($filename, $message) {
    if (@lstat($filename)) {
        print("lstat: $message.\n");
    }
    if (@stat($filename)) {
        print("stat: $message.\n");
    }
}

// Windows can use / for dir separators, so let's do that.
$qtestfile = str_replace("\\", "/", "$testfile");
passthru($php.' -n -r "touch(\\"'.$qtestfile.'\\");"');
all_the_stats("$testfile", "testfile exists");
passthru($php.' -n -r "unlink(\\"'.$qtestfile.'\\");"');
all_the_stats("$testfile", "testfile exists (it shouldn't)");
if (!@stat("$impossiblefile")) {
    print("stat impossiblefile does not exist.\n");
}
all_the_stats("$testfile", "testfile exists (it shouldn't)");
if (is_file("$phpfile")) {
    print("is_file(stat): php binary exists.\n");
}
all_the_stats("$testfile", "testfile exists (it shouldn't)");
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
