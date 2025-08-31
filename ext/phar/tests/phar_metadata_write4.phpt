--TEST--
Phar with object in metadata
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
class EchoesOnWakeup {
    public function __wakeup() {
        echo "In __wakeup " . spl_object_id($this) . "\n";
    }
    public function __destruct() {
        echo "In __destruct " . spl_object_id($this) . "\n";
    }
}
class ThrowsOnSerialize {
    public function __sleep() {
        throw new RuntimeException("In sleep");
    }
}
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a'] = array('cont' => 'a', 'meta' => new EchoesOnWakeup());
include 'files/phar_test.inc';

foreach($files as $name => $cont) {
    var_dump(file_get_contents($pname.'/'.$name));
}
unset($files);

$phar = new Phar($fname);
echo "Loading metadata for 'a' without allowed_classes\n";
var_dump($phar['a']->getMetadata(['allowed_classes' => []]));
echo "Loading metadata for 'a' with allowed_classes\n";
var_dump($phar['a']->getMetadata(['allowed_classes' => true]));
unset($phar);
// NOTE: Phar will use the cached value of metadata if setMetaData was called on that Phar path before.
// Save the writes to the phar and use a different file path.
$fname_new = "$fname.copy.php";
copy($fname, $fname_new);
$phar = new Phar($fname_new);
echo "Loading metadata from 'a' from the new phar\n";
var_dump($phar['a']->getMetadata());
echo "Loading metadata from 'a' from the new phar with unserialize options\n";
var_dump($phar['a']->getMetadata(['allowed_classes' => true]));
// PharEntry->setMetaData will do the following:
// 1. serialize, checking for exceptions
// 2. free the original data, checking for exceptions or the data getting set from destructors or error handlers.
// 3. set the new data.
try {
    var_dump($phar['a']->setMetadata(new ThrowsOnSerialize()));
} catch (RuntimeException $e) {
    echo "Caught {$e->getMessage()} at {$e->getFile()}:{$e->getLine()}\n";
    unset($e);
}
var_dump($phar['a']->getMetadata([]));
var_dump($phar['a']->getMetadata(['allowed_classes' => false]));

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php.copy.php');
?>
--EXPECTF--
In __destruct 1
string(1) "a"
Loading metadata for 'a' without allowed_classes
object(__PHP_Incomplete_Class)#3 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(14) "EchoesOnWakeup"
}
Loading metadata for 'a' with allowed_classes
In __wakeup 2
object(EchoesOnWakeup)#2 (0) {
}
In __destruct 2
Loading metadata from 'a' from the new phar
In __wakeup 3
object(EchoesOnWakeup)#3 (0) {
}
In __destruct 3
Loading metadata from 'a' from the new phar with unserialize options
In __wakeup 2
object(EchoesOnWakeup)#2 (0) {
}
In __destruct 2
Caught In sleep at %sphar_metadata_write4.php:12
In __wakeup 3
object(EchoesOnWakeup)#3 (0) {
}
In __destruct 3
object(__PHP_Incomplete_Class)#4 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(14) "EchoesOnWakeup"
}