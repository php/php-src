--TEST--
autoload_set_classmap Throw error if mapped file did not contain class
--FILE--
<?php

$tmp_file = sys_get_temp_dir() . '/ThisFileIsEmpty.php';
file_put_contents($tmp_file, '<?php ');

autoload_set_classmap(['foo' => $tmp_file]);

try {
    new Foo();
}
catch (\Throwable $ex) {
    echo get_class($ex) . ' - ' . $ex->getMessage();
}

?>
--EXPECTF--
Error - Error during autoloading from classmap. Entry "foo" failed to load the class from "%s/ThisFileIsEmpty.php" (Class undefined after file included).
