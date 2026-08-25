--TEST--
Bug #80056 (SPL directory iterators lose entries on filesystems that cannot rewind a directory)
--FILE--
<?php
/* Simulates a filesystem (e.g. 9p as used by WSL2 and Docker Desktop) where
 * seeking a directory handle after a partial read is silently ignored:
 * rewinddir() reports success but the read position is not reset. */
class BrokenSeekDir {
    public $context;
    private $entries = ['a.txt', 'b.txt', 'c.txt', 'd.txt', 'e.txt'];
    private $idx = 0;

    public function dir_opendir($path, $options): bool {
        $this->idx = 0;
        return true;
    }

    public function dir_readdir(): string|false {
        return $this->idx < count($this->entries) ? $this->entries[$this->idx++] : false;
    }

    public function dir_rewinddir(): bool {
        /* Broken on purpose: pretends to succeed without resetting the position. */
        return true;
    }

    public function dir_closedir(): bool {
        return true;
    }

    public function url_stat($path, $flags): array|false {
        return ['dev' => 0, 'ino' => 0, 'mode' => 0100644, 'nlink' => 1,
            'uid' => 0, 'gid' => 0, 'rdev' => -1, 'size' => 0,
            'atime' => 0, 'mtime' => 0, 'ctime' => 0, 'blksize' => -1, 'blocks' => -1];
    }
}
stream_wrapper_register('brokenseek', BrokenSeekDir::class);

echo "DirectoryIterator:\n";
$names = [];
foreach (new DirectoryIterator('brokenseek://dir') as $info) {
    $names[] = $info->getFilename();
}
var_dump($names);

echo "FilesystemIterator:\n";
$names = [];
$it = new FilesystemIterator('brokenseek://dir',
    FilesystemIterator::KEY_AS_FILENAME | FilesystemIterator::CURRENT_AS_PATHNAME);
foreach ($it as $name => $path) {
    $names[] = $name;
}
var_dump($names);

echo "RecursiveDirectoryIterator:\n";
$names = [];
$it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator('brokenseek://dir',
    FilesystemIterator::KEY_AS_FILENAME | FilesystemIterator::CURRENT_AS_PATHNAME));
foreach ($it as $name => $path) {
    $names[] = $name;
}
var_dump($names);

echo "Entry accessed before iteration:\n";
$it = new DirectoryIterator('brokenseek://dir');
var_dump($it->current()->getFilename());
$names = [];
foreach ($it as $info) {
    $names[] = $info->getFilename();
}
var_dump($names);
?>
--EXPECT--
DirectoryIterator:
array(5) {
  [0]=>
  string(5) "a.txt"
  [1]=>
  string(5) "b.txt"
  [2]=>
  string(5) "c.txt"
  [3]=>
  string(5) "d.txt"
  [4]=>
  string(5) "e.txt"
}
FilesystemIterator:
array(5) {
  [0]=>
  string(5) "a.txt"
  [1]=>
  string(5) "b.txt"
  [2]=>
  string(5) "c.txt"
  [3]=>
  string(5) "d.txt"
  [4]=>
  string(5) "e.txt"
}
RecursiveDirectoryIterator:
array(5) {
  [0]=>
  string(5) "a.txt"
  [1]=>
  string(5) "b.txt"
  [2]=>
  string(5) "c.txt"
  [3]=>
  string(5) "d.txt"
  [4]=>
  string(5) "e.txt"
}
Entry accessed before iteration:
string(5) "a.txt"
array(5) {
  [0]=>
  string(5) "a.txt"
  [1]=>
  string(5) "b.txt"
  [2]=>
  string(5) "c.txt"
  [3]=>
  string(5) "d.txt"
  [4]=>
  string(5) "e.txt"
}
