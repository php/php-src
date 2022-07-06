--TEST--
Phar with object in metadata
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a'] = array('cont' => 'a');
include 'files/phar_test.inc';

foreach($files as $name => $cont) {
    var_dump(file_get_contents($pname.'/'.$name));
}

$phar = new Phar($fname);
var_dump($phar->getMetadata());
$phar->setMetadata((object) ['my' => 'friend']);
unset($phar);
// NOTE: Phar will use the cached value of metadata if setMetaData was called on that Phar path before.
// Save the writes to the phar and use a different file path.
$fname_new = "$fname.copy.php";
copy($fname, $fname_new);
$phar = new Phar($fname_new);
var_dump($phar->getMetadata());

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php.copy.php');
?>
--EXPECT--
string(1) "a"
NULL
object(stdClass)#2 (1) {
  ["my"]=>
  string(6) "friend"
}
