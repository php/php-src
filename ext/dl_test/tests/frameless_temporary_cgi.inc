<?php
$ext = PHP_OS_FAMILY === 'Windows' ? 'php_dl_test.dll' : 'dl_test.so';
if (!dl($ext)) {
    echo "dl failed\n";
    return;
}
include __DIR__ . '/frameless_temporary.inc';
