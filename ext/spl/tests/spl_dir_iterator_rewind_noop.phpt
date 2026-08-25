--TEST--
SPL directory iterators only seek the directory stream when entries have been consumed
--FILE--
<?php
class LoggingDir {
    public static $rewinds = 0;
    public $context;
    private $entries = ['a.txt', 'b.txt', 'c.txt'];
    private $idx = 0;

    public function dir_opendir($path, $options): bool {
        $this->idx = 0;
        return true;
    }

    public function dir_readdir(): string|false {
        return $this->idx < count($this->entries) ? $this->entries[$this->idx++] : false;
    }

    public function dir_rewinddir(): bool {
        self::$rewinds++;
        $this->idx = 0;
        return true;
    }

    public function dir_closedir(): bool {
        return true;
    }
}
stream_wrapper_register('logdir', LoggingDir::class);

echo "First iteration needs no seek:\n";
$it = new FilesystemIterator('logdir://dir',
    FilesystemIterator::KEY_AS_FILENAME | FilesystemIterator::CURRENT_AS_PATHNAME);
$names = [];
foreach ($it as $name => $path) {
    $names[] = $name;
}
var_dump($names, LoggingDir::$rewinds);

echo "Iterating again performs a real rewind:\n";
$names = [];
foreach ($it as $name => $path) {
    $names[] = $name;
}
var_dump($names, LoggingDir::$rewinds);

echo "Explicit rewind after next() performs a real rewind:\n";
LoggingDir::$rewinds = 0;
$it = new DirectoryIterator('logdir://dir');
$it->next();
$it->rewind();
$names = [];
while ($it->valid()) {
    $names[] = $it->getFilename();
    $it->next();
}
var_dump($names, LoggingDir::$rewinds);

echo "Repeated rewind without reads stays a no-op:\n";
LoggingDir::$rewinds = 0;
$it = new DirectoryIterator('logdir://dir');
$it->rewind();
$it->rewind();
$names = [];
foreach ($it as $info) {
    $names[] = $info->getFilename();
}
var_dump($names, LoggingDir::$rewinds);
?>
--EXPECT--
First iteration needs no seek:
array(3) {
  [0]=>
  string(5) "a.txt"
  [1]=>
  string(5) "b.txt"
  [2]=>
  string(5) "c.txt"
}
int(0)
Iterating again performs a real rewind:
array(3) {
  [0]=>
  string(5) "a.txt"
  [1]=>
  string(5) "b.txt"
  [2]=>
  string(5) "c.txt"
}
int(1)
Explicit rewind after next() performs a real rewind:
array(3) {
  [0]=>
  string(5) "a.txt"
  [1]=>
  string(5) "b.txt"
  [2]=>
  string(5) "c.txt"
}
int(1)
Repeated rewind without reads stays a no-op:
array(3) {
  [0]=>
  string(5) "a.txt"
  [1]=>
  string(5) "b.txt"
  [2]=>
  string(5) "c.txt"
}
int(0)
