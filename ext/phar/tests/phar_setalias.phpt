--TEST--
Phar::setAlias()
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

$phar = new Phar($fname);
echo $phar->getAlias() . "\n";
$phar->setAlias('test');
echo $phar->getAlias() . "\n";
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
?>
--EXPECT--
hio
test
