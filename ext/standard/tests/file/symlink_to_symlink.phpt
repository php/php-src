--TEST--
symlink() using a relative path, and symlink() to a symlink
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not for Windows');
}
?>
--FILE--
<?php
$prefix = __FILE__;

touch($prefix . "_file");

// symlink to a regular file using a relative dest
symlink(basename($prefix . "_file"), $prefix . "_link1");

// symlink to a symlink using a relative path
symlink(basename($prefix . "_link1"), $prefix . "_link2");

// symlink to a non-existent path
@unlink($prefix . "_nonexistent");
symlink(basename($prefix . "_nonexistent"), $prefix . "_link3");

// symlink to a regular file using an absolute path
symlink($prefix . "_file", $prefix . "_link4");

// symlink to a symlink using an absolute path
symlink($prefix . "_link4", $prefix . "_link5");

var_dump(readlink($prefix . "_link1"));
var_dump(readlink($prefix . "_link2"));
var_dump(readlink($prefix . "_link3"));
var_dump(readlink($prefix . "_link4"));
var_dump(readlink($prefix . "_link5"));

unlink($prefix . "_link5");
unlink($prefix . "_link4");
unlink($prefix . "_link3");
unlink($prefix . "_link2");
unlink($prefix . "_link1");
unlink($prefix . "_file");

?>
--EXPECTF--
string(%d) "symlink_to_symlink.php_file"
string(%d) "symlink_to_symlink.php_link1"
string(%d) "symlink_to_symlink.php_nonexistent"
string(%d) "%s/symlink_to_symlink.php_file"
string(%d) "%s/symlink_to_symlink.php_link4"
