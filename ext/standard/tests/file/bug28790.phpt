--TEST--
Bug #28790: Add php.ini option to disable stat cache
--CREDITS--
jnoll <jnoll [at] prim [dot] hu>
Kevin Lyda <kevin [at] lyda [dot] ie>
--FILE--
<?php

    $inifile = __DIR__.DIRECTORY_SEPARATOR.'bug28790.ini';
    $php = '"'.getenv('TEST_PHP_EXECUTABLE').'"';
    $phpfile = getenv('TEST_PHP_EXECUTABLE');
    $impossiblefile = $phpfile.DIRECTORY_SEPARATOR.'bug28790.impossible';
    $testfile = __DIR__.DIRECTORY_SEPARATOR.'bug28790.file';
    $codefile = __DIR__.DIRECTORY_SEPARATOR.'bug28790.test.php';
    $code = <<<EOC
<?php

function all_the_stats(\$filename, \$message) {
    if (@lstat(\$filename)) {
        print("lstat: \$message.\n");
    }
    if (@stat(\$filename)) {
        print("stat: \$message.\n");
    }
}

\$testfile = str_replace("\\\\", "/", "$testfile");
passthru('$php -n -r \'touch("\$testfile");\'');
all_the_stats("$testfile", "testfile exists");
passthru('$php -n -r \'unlink("\$testfile");\'');
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
EOC;
    $f = fopen($codefile, "w");
    fwrite($f, $code);
    fclose($f);

    $code = '"var_dump(php_ini_loaded_file());"';

    // No ini file
    passthru($php.' -n '.$codefile);

    print("---running with disable_stat_cache = True---\n");
    // Specified ini file
    passthru($php.' -c "'.$inifile.'" '.$codefile);

    unlink($codefile);

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
---running with disable_stat_cache = True---
lstat: testfile exists.
stat: testfile exists.
stat impossiblefile does not exist.
is_file(stat): php binary exists.
lstat: php binary exists.
