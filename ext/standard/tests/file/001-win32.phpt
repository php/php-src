--TEST--
File type functions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
?>
--POST--
--GET--
--FILE--
<?php
chdir(dirname(__FILE__));
@unlink('test.file');
@unlink('test.link');
if (file_exists('test.file')) {
    echo "test.file exists\n";
} else {
    echo "test.file does not exist\n";
}
fclose (fopen('test.file', 'w'));
chmod ('test.file', 0744);
if (file_exists('test.file')) {
    echo "test.file exists\n";
} else {
    echo "test.file does not exist\n";
}
sleep (2);
if (file_exists('test.link')) {
    echo "test.link exists\n";
} else {
    echo "test.link does not exist\n";
}
if (is_link('test.file')) {
    echo "test.file is a symlink\n";
} else {
    echo "test.file is not a symlink\n";
}
if (is_link('test.link')) {
    echo "test.link is a symlink\n";
} else {
    echo "test.link is not a symlink\n";
}
if (file_exists('test.file')) {
    echo "test.file exists\n";
} else {
    echo "test.file does not exist\n";
}
$s = stat ('test.file');
$ls = lstat ('test.file');
for ($i = 0; $i <= 12; $i++) {
    if ($ls[$i] != $s[$i]) {
        echo "test.file lstat and stat differ at element $i\n";
    }
}
echo "test.file is " . filetype('test.file') . "\n";
echo "test.link is " . filetype('test.link') . "\n";
printf ("test.file permissions are 0%o\n", 0777 & fileperms('test.file'));
echo "test.file size is " . filesize('test.file') . "\n";
if (is_writeable('test.file')) {
    echo "test.file is writeable\n";
} else {
    echo "test.file is not writeable\n";
}
if (is_readable('test.file')) {
    echo "test.file is readable\n";
} else {
    echo "test.file is not readable\n";
}
if (is_file('test.file')) {
    echo "test.file is a regular file\n";
} else {
    echo "test.file is not a regular file\n";
}
if (is_file('test.link')) {
    echo "test.link is a regular file\n";
} else {
    echo "test.link is not a regular file\n";
}
if (is_dir('test.link')) {
    echo "test.link is a directory\n";
} else {
    echo "test.link is not a directory\n";
}
if (is_dir('../file')) {
    echo "../file is a directory\n";
} else {
    echo "../file is not a directory\n";
}
if (is_dir('test.file')) {
    echo "test.file is a directory\n";
} else {
    echo "test.file is not a directory\n";
}
unlink('test.file');
if (file_exists('test.file')) {
    echo "test.file exists (cached)\n";
} else {
    echo "test.file does not exist\n";
}
clearstatcache();
if (file_exists('test.file')) {
    echo "test.file exists\n";
} else {
    echo "test.file does not exist\n";
}
?>
--EXPECT--
test.file does not exist
test.file exists
test.link does not exist
test.file is not a symlink
test.link is not a symlink
test.file exists
test.file is file
test.link is file
test.file permissions are 0666
test.file size is 0
test.file is writeable
test.file is readable
test.file is a regular file
test.link is not a regular file
test.link is not a directory
../file is a directory
test.file is not a directory
test.file does not exist
test.file does not exist
