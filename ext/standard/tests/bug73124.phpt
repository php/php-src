--TEST--
Bug #73124 (php_ini_scanned_files relied on PHP_CONFIG_FILE_SCAN_DIR)
--SKIPIF--
<?php
if (!empty(PHP_CONFIG_FILE_SCAN_DIR)) die("Skip: PHP_CONFIG_FILE_SCAN_DIR must not be available");
?>
--FILE--
<?php
    $tempDir = sys_get_temp_dir();
    putenv('PHP_INI_SCAN_DIR='.$tempDir);

    $inifile = $tempDir.DIRECTORY_SEPARATOR.'scan-dir.ini';
    @unlink($inifile);
    file_put_contents($inifile, "\n");

    $php = getenv('TEST_PHP_EXECUTABLE');
    passthru('"'.$php.'" -r "print_r(php_ini_scanned_files());"');

    putenv('PHP_INI_SCAN_DIR=');
    @unlink($inifile);
?>
--EXPECTREGEX--
.*[\/\\]scan-dir\.ini.*|.*[\/\\]scan-dir\.ini
Done
