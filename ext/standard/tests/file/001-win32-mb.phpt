--TEST--
File type functions
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
?>
--FILE--
<?php
chdir(dirname(__FILE__));
@unlink('test_私はガラスを食べられます.file');
if (file_exists('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file exists\n";
} else {
    echo "test_私はガラスを食べられます.file does not exist\n";
}
fclose (fopen('test_私はガラスを食べられます.file', 'w'));
chmod ('test_私はガラスを食べられます.file', 0744);
if (file_exists('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file exists\n";
} else {
    echo "test_私はガラスを食べられます.file does not exist\n";
}
if (is_link('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file is a symlink\n";
} else {
    echo "test_私はガラスを食べられます.file is not a symlink\n";
}
if (file_exists('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file exists\n";
} else {
    echo "test_私はガラスを食べられます.file does not exist\n";
}
$s = stat ('test_私はガラスを食べられます.file');
$ls = lstat ('test_私はガラスを食べられます.file');
for ($i = 0; $i <= 12; $i++) {
    if ($ls[$i] != $s[$i]) {
        echo "test_私はガラスを食べられます.file lstat and stat differ at element $i\n";
    }
}
echo "test_私はガラスを食べられます.file is " . filetype('test_私はガラスを食べられます.file') . "\n";
printf ("test_私はガラスを食べられます.file permissions are 0%o\n", 0777 & fileperms('test_私はガラスを食べられます.file'));
echo "test_私はガラスを食べられます.file size is " . filesize('test_私はガラスを食べられます.file') . "\n";
if (is_writeable('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file is writeable\n";
} else {
    echo "test_私はガラスを食べられます.file is not writeable\n";
}
if (is_readable('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file is readable\n";
} else {
    echo "test_私はガラスを食べられます.file is not readable\n";
}
if (is_file('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file is a regular file\n";
} else {
    echo "test_私はガラスを食べられます.file is not a regular file\n";
}
if (is_dir('../file')) {
    echo "../file is a directory\n";
} else {
    echo "../file is not a directory\n";
}
if (is_dir('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file is a directory\n";
} else {
    echo "test_私はガラスを食べられます.file is not a directory\n";
}
unlink('test_私はガラスを食べられます.file');
if (file_exists('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file exists (cached)\n";
} else {
    echo "test_私はガラスを食べられます.file does not exist\n";
}
clearstatcache();
if (file_exists('test_私はガラスを食べられます.file')) {
    echo "test_私はガラスを食べられます.file exists\n";
} else {
    echo "test_私はガラスを食べられます.file does not exist\n";
}
?>
--EXPECT--
test_私はガラスを食べられます.file does not exist
test_私はガラスを食べられます.file exists
test_私はガラスを食べられます.file is not a symlink
test_私はガラスを食べられます.file exists
test_私はガラスを食べられます.file is file
test_私はガラスを食べられます.file permissions are 0666
test_私はガラスを食べられます.file size is 0
test_私はガラスを食べられます.file is writeable
test_私はガラスを食べられます.file is readable
test_私はガラスを食べられます.file is a regular file
../file is a directory
test_私はガラスを食べられます.file is not a directory
test_私はガラスを食べられます.file does not exist
test_私はガラスを食べられます.file does not exist

