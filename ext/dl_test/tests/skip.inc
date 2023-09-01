<?php

// Check that the dl_test extension is built. We don't use the --EXTENSIONS--
// section because we want to load the extension with dl().

if (PHP_OS_FAMILY === 'Windows') {
    $path = ini_get('extension_dir') . DIRECTORY_SEPARATOR . 'php_dl_test.dll';
} else {
    $path = ini_get('extension_dir') . DIRECTORY_SEPARATOR . 'dl_test.so';
}

if (!file_exists($path)) {
    die(sprintf('skip dl_test extension is not built (tried %s)', $path));
}

if (getenv('SKIP_ASAN')) {
    die('skip dl() crashes LSan');
}
