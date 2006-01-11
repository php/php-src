--TEST--
Phar::loadPhar ignoring alias
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
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
$alias = 'alias';
$manifest = pack('VnV', count($files), 0x0800, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= $cont;
}

file_put_contents($fname, $file);

Phar::loadPhar($fname);

require $pname . '/a.php';

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
?>
--EXPECTF--
This is a
This is b
This is b/c
This is b/d
This is e
===DONE===
