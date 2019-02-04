--TEST--
Userstream unlink, rename, mkdir, rmdir, and url_stat.
--FILE--
<?php
class test {
    function unlink($file) {
        print "Unlinking file: $file\n";
    }

    function rename($from, $to) {
        print "Renaming $from to $to\n";
    }

    function mkdir($directory, $mode, $options) {
        printf("Making directory: %s as %o%s\n", $directory, $mode, $options & STREAM_MKDIR_RECURSIVE ? " recursively" : "");
    }

    function rmdir($directory, $options) {
        print "Removing directory: $directory\n";
    }

    function url_stat($path, $options) {
		/* By printing out a notice that we are actively stating the file
		   then subsequently performing multiple stat operations on it
		   we effectively test the stat cache mechanism */
        print "Stating file: $path\n";
        return array('dev'=>1, 'ino'=>2, 'mode'=>0644, 'nlink'=>3,
                     'uid'=>100, 'gid'=>1000, 'rdev'=>-1, 'size'=>31337,
                     'atime'=>1234567890, 'mtime'=>1231231231, 'ctime'=>1234564564,
                     'blksize'=>-1, 'blocks'=>-1);
    }
}

stream_wrapper_register('test', 'test');

unlink('test://example.com/path/to/file');
rename('test://example.com/path/to/from', 'test://example.com/path/to/to');
/* We *want* this to fail and thus not output the watch statement */
@rename('test://example.com/path/to/from', 'http://example.com/path/to/to');
mkdir('test://example.com/path/to/directory', 0755);
rmdir('test://example.com/path/to/directory');
print_r(stat('test://example.com/path/to/file'));
echo "Filesize = " . filesize('test://example.com/path/to/file') . "\n";
echo "filemtime = " . filemtime('test://example.com/path/to/file') . "\n";
?>
--EXPECT--
Unlinking file: test://example.com/path/to/file
Renaming test://example.com/path/to/from to test://example.com/path/to/to
Making directory: test://example.com/path/to/directory as 755
Removing directory: test://example.com/path/to/directory
Stating file: test://example.com/path/to/file
Array
(
    [0] => 1
    [1] => 2
    [2] => 420
    [3] => 3
    [4] => 100
    [5] => 1000
    [6] => -1
    [7] => 31337
    [8] => 1234567890
    [9] => 1231231231
    [10] => 1234564564
    [11] => -1
    [12] => -1
    [dev] => 1
    [ino] => 2
    [mode] => 420
    [nlink] => 3
    [uid] => 100
    [gid] => 1000
    [rdev] => -1
    [size] => 31337
    [atime] => 1234567890
    [mtime] => 1231231231
    [ctime] => 1234564564
    [blksize] => -1
    [blocks] => -1
)
Filesize = 31337
filemtime = 1231231231
