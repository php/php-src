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
chdir(__DIR__);

$fname = 'test_私はガラスを食べられます.file';

@unlink($fname);
if (file_exists($fname)) {
    echo "$fname exists\n";
} else {
    echo "$fname does not exist\n";
}
fclose (fopen($fname, 'w'));
chmod ($fname, 0744);
if (file_exists($fname)) {
    echo "$fname exists\n";
} else {
    echo "$fname does not exist\n";
}
if (is_link($fname)) {
    echo "$fname is a symlink\n";
} else {
    echo "$fname is not a symlink\n";
}
if (file_exists($fname)) {
    echo "$fname exists\n";
} else {
    echo "$fname does not exist\n";
}
$s = stat ($fname);
$ls = lstat ($fname);
for ($i = 0; $i <= 12; $i++) {
    if ($ls[$i] != $s[$i]) {
        echo "$fname lstat and stat differ at element $i\n";
    }
}
echo "$fname is " . filetype($fname) . "\n";
printf ("$fname permissions are 0%o\n", 0777 & fileperms($fname));
echo "$fname size is " . filesize($fname) . "\n";
if (is_writeable($fname)) {
    echo "$fname is writeable\n";
} else {
    echo "$fname is not writeable\n";
}
if (is_readable($fname)) {
    echo "$fname is readable\n";
} else {
    echo "$fname is not readable\n";
}
if (is_file($fname)) {
    echo "$fname is a regular file\n";
} else {
    echo "$fname is not a regular file\n";
}
if (is_dir('../file')) {
    echo "../file is a directory\n";
} else {
    echo "../file is not a directory\n";
}
if (is_dir($fname)) {
    echo "$fname is a directory\n";
} else {
    echo "$fname is not a directory\n";
}
unlink($fname);
if (file_exists($fname)) {
    echo "$fname exists (cached)\n";
} else {
    echo "$fname does not exist\n";
}
clearstatcache();
if (file_exists($fname)) {
    echo "$fname exists\n";
} else {
    echo "$fname does not exist\n";
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
