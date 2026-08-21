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
        echo "VALUE_ERROR\n";
    } catch (Throwable $e) {
        echo "ERROR\n";
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
VALUE_ERROR
VALUE_ERROR
VALUE_ERROR
--CLEAN--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'test.db';
@unlink($filename);
?>
