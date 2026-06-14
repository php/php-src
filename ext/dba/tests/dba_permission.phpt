--TEST--
DBA permission validation (invalid bits check)
--EXTENSIONS--
dba
--SKIPIF--
<?php
if (!in_array('flatfile', dba_handlers())) die('skip flatfile handler not available');
?>
--FILE--
<?php

$filename = __DIR__ . DIRECTORY_SEPARATOR . 'test.db';

function test($permission, $filename) {
    try {
        dba_open($filename, "c", "flatfile", $permission);
        echo "OK\n";
    } catch (ValueError $e) {
        echo $e->getMessage() . PHP_EOL;
    }

    @unlink($filename);
}

/* valid permissions */
test(0777, $filename);
test(0755, $filename);
test(0644, $filename);
test(0000, $filename);

/* invalid permissions */
test(010000, $filename);
test(020000, $filename);
test(-1, $filename);

?>
--EXPECT--
OK
OK
OK
OK
dba_open(): Argument #4 ($permission) Invalid file permission value (must be between 0 and 07777)
dba_open(): Argument #4 ($permission) Invalid file permission value (must be between 0 and 07777)
dba_open(): Argument #4 ($permission) Invalid file permission value (must be between 0 and 07777)
--CLEAN--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'test.db';
@unlink($filename);
?>