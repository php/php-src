--TEST--
Phar: .phar-prefixed non-magic directories are accessible
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);
$phar['.pharx/array.txt'] = 'array';
$phar->addFromString('.pharx/from-string.txt', 'from-string');
$phar->addFromString('/.phary/leading.txt', 'leading');
$phar->copy('.pharx/array.txt', '.pharx/copy.txt');

var_dump(isset($phar['.pharx/array.txt']));
echo $phar['.pharx/array.txt']->getContent(), "\n";
echo file_get_contents($pname . '/.pharx/from-string.txt'), "\n";
echo file_get_contents($pname . '/.phary/leading.txt'), "\n";
echo file_get_contents($pname . '/.pharx/copy.txt'), "\n";

$root = [];
$dh = opendir($pname . '/');
while (false !== ($entry = readdir($dh))) {
    $root[] = $entry;
}
closedir($dh);
sort($root);
var_dump($root);

$subdir = [];
$dh = opendir($pname . '/.pharx');
while (false !== ($entry = readdir($dh))) {
    $subdir[] = $entry;
}
closedir($dh);
sort($subdir);
var_dump($subdir);

try {
    $phar->addFromString('.phar/still-magic.txt', 'no');
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $phar->addEmptyDir('/.phar');
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
?>
--EXPECT--
bool(true)
array
from-string
leading
array
array(2) {
  [0]=>
  string(6) ".pharx"
  [1]=>
  string(6) ".phary"
}
array(3) {
  [0]=>
  string(9) "array.txt"
  [1]=>
  string(8) "copy.txt"
  [2]=>
  string(15) "from-string.txt"
}
Cannot create any files in magic ".phar" directory
Cannot create a directory in magic ".phar" directory
