--TEST--
Phar: phar:// include with Opcache
--EXTENSIONS--
phar
opcache
--SKIPIF--
<?php
if (strpos(PHP_OS, 'WIN') === false) die("skip Extra warning on Windows.");
$cache_dir = dirname(__FILE__) . "/024-file_cache";
if (!is_dir($cache_dir) && !mkdir($cache_dir)) die("skip unable to create file_cache dir");
?>
--INI--
phar.require_hash=0
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache={PWD}/024-file_cache
opcache.memory_consumption=64
opcache.interned_strings_buffer=8
opcache.max_accelerated_files=4000
opcache.jit=tracing
opcache.revalidate_freq=60
opcache.fast_shutdown=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';

include 'files/phar_test.inc';

include $pname . '/a.php';
include $pname . '/b.php';
include $pname . '/b/c.php';

$cache_dir = ini_get("opcache.file_cache");
if (is_dir($cache_dir)) {
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($cache_dir, RecursiveDirectoryIterator::SKIP_DOTS), RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($it as $fi) {
        $fn = ($fi->isDir() ? 'rmdir' : 'unlink');
        $fn($fi->getRealPath());
    }

    rmdir($cache_dir);
}

?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
This is a
This is b
This is b/c
