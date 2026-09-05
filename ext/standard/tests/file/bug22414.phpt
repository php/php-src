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

    $php_escaped = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
    $source = tempnam(__DIR__, 'phpt');
    $tmpfile = tempnam(__DIR__, 'phpt');
    $args = ' -n ';

    /* Regular Data Test */
    passthru($php_escaped . $args . ' -r " echo \"HELLO\"; "');

    echo "\n";

    /* Binary Data Test */
    $binaryData = '';
    for ($i = 0; $i < 256; $i++) {
        $binaryData .= chr($i);
    }
    file_put_contents($source, str_repeat($binaryData, 1024));
    putenv('TEST_PHP_PASSTHRU_SOURCE=' . $source);

    $cmd = $php_escaped . $args . ' -r ' . escapeshellarg("readfile(@getenv('TEST_PHP_PASSTHRU_SOURCE'));");
    if (substr(PHP_OS, 0, 3) != 'WIN') {
        $cmd = $php_escaped . $args . ' -r ' . escapeshellarg('passthru("'.$cmd.'");') . ' > '.escapeshellarg($tmpfile);
    } else {
        $cmd = $php_escaped . $args . ' -r ' . "\"passthru('".addslashes($cmd)."');\"" . ' > '.escapeshellarg($tmpfile);
    }
    exec($cmd);

    if (md5_file($source) == md5_file($tmpfile)) {
        echo "Works\n";
    } else {
        echo "Does not work\n";
    }

    @unlink($source);
    @unlink($tmpfile);
?>
--EXPECT--
HELLO
Works
