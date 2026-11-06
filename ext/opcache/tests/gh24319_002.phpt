--TEST--
GH-24319: Existing OPcache aliases follow later direct-to-indirect conversions
--DESCRIPTION--
An alias can already point to a direct cache entry when that entry is converted
into another alias. Lookups must resolve the resulting chain, including after
the final target is recompiled. Script enumeration must distinguish direct
entries from aliases using a consistent snapshot. A conversion whose normalized
target is the entry itself must not create an alias cycle. Protected shared
memory is enabled to ensure that read-side atomic operations do not write.
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip Unix symlinks required');
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_update_protection=0
opcache.validate_timestamps=0
opcache.revalidate_path=0
opcache.protect_memory=1
--FILE--
<?php
$dir = __DIR__ . '/gh24319_002.' . getmypid();
mkdir($dir);
foreach (['a' => 1, 'b' => 2, 'c' => 3] as $name => $value) {
    file_put_contents("$dir/$name.php", "<?php return $value;\n");
}
var_dump(require "$dir/a.php");
symlink("$dir/a.php", "$dir/alias.php");
var_dump(require "$dir/alias.php");

foreach (['a' => 'b', 'b' => 'c'] as $old => $new) {
    opcache_invalidate("$dir/$old.php", true);
    symlink("$dir/$new.php", "$dir/link");
    rename("$dir/link", "$dir/$old.php");
    clearstatcache(true);
    var_dump(require "$dir/$old.php");
    var_dump(require "$dir/alias.php");
}
$scripts = opcache_get_status()['scripts'];
var_dump(isset($scripts["$dir/c.php"]));
var_dump(isset($scripts["$dir/a.php"]), isset($scripts["$dir/b.php"]), isset($scripts["$dir/alias.php"]));

// Replacing the final script must also update every existing alias chain.
file_put_contents("$dir/c.php", "<?php return 6;\n");
opcache_invalidate("$dir/c.php", true);
var_dump(require "$dir/alias.php");
var_dump(require "$dir/a.php");
var_dump(require "$dir/b.php");
var_dump(require "$dir/c.php");

// Turn the original target into an alias of a name that previously pointed
// back to it. The writer must not create a self-referential cache entry.
file_put_contents("$dir/x.php", "<?php return 4;\n");
var_dump(require "$dir/x.php");
symlink("$dir/x.php", "$dir/y.php");
var_dump(require "$dir/y.php");
opcache_invalidate("$dir/x.php", true);
unlink("$dir/y.php");
file_put_contents("$dir/y.php", "<?php return 5;\n");
symlink("$dir/y.php", "$dir/link");
rename("$dir/link", "$dir/x.php");
clearstatcache(true);
var_dump(require "$dir/x.php");
var_dump(require "$dir/y.php");

foreach (glob($dir . '/*') as $file) unlink($file);
rmdir($dir);
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/gh24319_002.*', GLOB_ONLYDIR) ?: [] as $dir) {
    foreach (glob($dir . '/*') ?: [] as $file) unlink($file);
    rmdir($dir);
}
?>
--EXPECT--
int(1)
int(1)
int(2)
int(2)
int(3)
int(3)
bool(true)
bool(false)
bool(false)
bool(false)
int(6)
int(6)
int(6)
int(6)
int(4)
int(4)
int(5)
int(5)
