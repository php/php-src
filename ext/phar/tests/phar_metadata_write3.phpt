--TEST--
Phar with unsafe object in metadata does not unserialize on reading a file.
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
class EchoesOnWakeup {
    public function __wakeup() {
        echo "In wakeup\n";
    }
}
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a'] = array('cont' => 'contents of file a');
include 'files/phar_test.inc';

echo "Reading file contents through stream wrapper\n";
foreach($files as $name => $cont) {
    var_dump(file_get_contents($pname.'/'.$name));
}

$phar = new Phar($fname);
echo "Original metadata\n";
var_dump($phar->getMetadata());
$phar->setMetadata(new EchoesOnWakeup());
unset($phar);
// NOTE: Phar will use the cached value of metadata if setMetaData was called on that Phar path before.
// Save the writes to the phar and use a different file path.
$fname_new = "$fname.copy.php";
copy($fname, $fname_new);
$phar = new Phar($fname_new);
echo "Calling getMetadata\n";
var_dump($phar->getMetadata());
echo "Calling getMetadata with no allowed_classes\n";
var_dump($phar->getMetadata(['allowed_classes' => []]));
echo "Calling getMetadata with EchoesOnWakeup allowed\n";
var_dump($phar->getMetadata(['allowed_classes' => [EchoesOnWakeup::class]]));
// Part of this is a test that there are no unexpected behaviors when both selMetadata and getMetadata are used
$phar->setMetaData([new EchoesOnWakeup(), new stdClass()]);
echo "Calling getMetadata with too low max_depth\n";
var_dump($phar->getMetadata(['max_depth' => 1]));
echo "Calling getMetadata with some allowed classes\n";
var_dump($phar->getMetadata(['allowed_classes' => [EchoesOnWakeup::class]]));
echo "Calling getMetadata with no options returns the original metadata value\n";
var_dump($phar->getMetadata());
unset($phar);

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php.copy.php');
?>
--EXPECTF--
Reading file contents through stream wrapper
string(18) "contents of file a"
Original metadata
NULL
Calling getMetadata
In wakeup
object(EchoesOnWakeup)#2 (0) {
}
Calling getMetadata with no allowed_classes
object(__PHP_Incomplete_Class)#2 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(14) "EchoesOnWakeup"
}
Calling getMetadata with EchoesOnWakeup allowed
In wakeup
object(EchoesOnWakeup)#2 (0) {
}
Calling getMetadata with too low max_depth

Warning: Phar::getMetadata(): Maximum depth of 1 exceeded. The depth limit can be changed using the max_depth unserialize() option or the unserialize_max_depth ini setting in %sphar_metadata_write3.php on line 39

Notice: Phar::getMetadata(): Error at offset 34 of 59 bytes in %sphar_metadata_write3.php on line 39
bool(false)
Calling getMetadata with some allowed classes
In wakeup
array(2) {
  [0]=>
  object(EchoesOnWakeup)#4 (0) {
  }
  [1]=>
  object(__PHP_Incomplete_Class)#5 (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(8) "stdClass"
  }
}
Calling getMetadata with no options returns the original metadata value
array(2) {
  [0]=>
  object(EchoesOnWakeup)#2 (0) {
  }
  [1]=>
  object(stdClass)#3 (0) {
  }
}
