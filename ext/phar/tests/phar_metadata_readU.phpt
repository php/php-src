--TEST--
Phar with metadata (read)
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "<")) die("skip Unicode support required");
?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$pmeta = 'hi there';
$files['a'] = array('cont' => 'a');
$files['b'] = array('cont' => 'b');
$files['c'] = array('cont' => 'c', 'meta' => array('hi', 'there'));
$files['d'] = array('cont' => 'd', 'meta' => array('hi'=>'there','foo'=>'bar'));
include 'files/phar_test.inc';

foreach($files as $name => $cont) {
	var_dump(file_get_contents($pname.'/'.$name));
}

$phar = new Phar($fname);
var_dump($phar->hasMetaData());
var_dump($phar->getMetaData());
var_dump($phar->delMetaData());
var_dump($phar->getMetaData());
var_dump($phar->delMetaData());
var_dump($phar->getMetaData());
foreach($files as $name => $cont) {
	echo "  meta $name\n";
	var_dump($phar[$name]->hasMetadata());
	var_dump($phar[$name]->getMetadata());
	var_dump($phar[$name]->delMetadata());
	var_dump($phar[$name]->getMetadata());
}

unset($phar);

foreach($files as $name => $cont) {
	var_dump(file_get_contents($pname.'/'.$name));
}
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
string(1) "a"
string(1) "b"
string(1) "c"
string(1) "d"
bool(true)
unicode(8) "hi there"
bool(true)
NULL
bool(true)
NULL
  meta a
bool(false)
NULL
bool(true)
NULL
  meta b
bool(false)
NULL
bool(true)
NULL
  meta c
bool(true)
array(2) {
  [0]=>
  unicode(2) "hi"
  [1]=>
  unicode(5) "there"
}
bool(true)
NULL
  meta d
bool(true)
array(2) {
  [u"hi"]=>
  unicode(5) "there"
  [u"foo"]=>
  unicode(3) "bar"
}
bool(true)
NULL
string(1) "a"
string(1) "b"
string(1) "c"
string(1) "d"
===DONE===
