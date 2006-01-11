--TEST--
Phar::loadPhar overloading alias names
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
$fname1 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.1.phar.php';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.2.phar.php';
$pname = 'phar://test';
$file = '<?php include "' . $pname . '/a.php"; __HALT_COMPILER(); ?>';

$files = array();
$files['a.php']   = '<?php echo "This is a\n"; include "'.$pname.'/b.php"; ?>';      
$files['b.php']   = '<?php echo "This is b\n"; include "'.$pname.'/b/c.php"; ?>';    
$files['b/c.php'] = '<?php echo "This is b/c\n"; include "'.$pname.'/b/d.php"; ?>';  
$files['b/d.php'] = '<?php echo "This is b/d\n"; include "'.$pname.'/e.php"; ?>';    
$files['e.php']   = '<?php echo "This is e\n"; ?>';                                  

$manifest = '';
foreach($files as $name => $cont) {
	$len = strlen($cont);
	$manifest .= pack('V', strlen($name)) . $name . pack('VVVVC', $len, time(), $len, crc32($cont), 0x00);
}
$alias = '';
$manifest = pack('VnV', count($files), 0x0800, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= $cont;
}

file_put_contents($fname1, $file);
file_put_contents($fname2, $file);

var_dump(Phar::loadPhar($fname1, 'test'));
var_dump(Phar::loadPhar($fname1, 'copy'));
var_dump(Phar::loadPhar($fname2, 'copy'));

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.1.phar.php');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.phar.php');
?>
--EXPECTF--
bool(true)
bool(true)

Fatal error: Phar::loadPhar(): alias "copy" is already used for archive "%s029.1.phar.php" cannot be overloaded with "%s029.2.phar.php" in %s029.php on line %d
