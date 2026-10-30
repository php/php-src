--TEST--
Bug #22414 (passthru() does not read data correctly)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--INI--
output_handler=
--FILE--
<?php

    $php = getenv('TEST_PHP_EXECUTABLE');
    $php_escaped = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
    $tmpfile = tempnam(__DIR__, 'phpt');
    $args = ' -n ';

    /* Regular Data Test */
    passthru($php_escaped . $args . ' -r " echo \"HELLO\"; "');

    echo "\n";

    /* Binary Data Test */
    $cmd = $php_escaped . $args . ' -r ' . escapeshellarg("readfile(@getenv('TEST_PHP_EXECUTABLE'));");
    if (substr(PHP_OS, 0, 3) != 'WIN') {
        $cmd = $php_escaped . $args . ' -r ' . escapeshellarg('passthru("'.$cmd.'");') . ' > '.escapeshellarg($tmpfile);
    } else {
        $cmd = $php_escaped . $args . ' -r ' . "\"passthru('".addslashes($cmd)."');\"" . ' > '.escapeshellarg($tmpfile);
    }
    exec($cmd);

    if (md5_file($php) == md5_file($tmpfile)) {
        echo "Works\n";
    } else {
        echo "Does not work\n";
    }

    @unlink($tmpfile);
?>
--EXPECT--
HELLO
Works
