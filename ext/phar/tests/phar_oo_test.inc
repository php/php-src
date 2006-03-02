<?php

ini_set('date.timezone', 'GMT');

$fname = dirname(__FILE__) . '/phar_oo_test.phar.php';
$pname = 'phar://' . $fname;
$file = '<?php include "' . $pname . '/a.php"; __HALT_COMPILER(); ?>';

$files = array();
$files['a.php']   = '<?php echo "This is a.php\n"; ?>';
$files['b.php']   = '<?php echo "This is b.php\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c.php\n"; ?>';
$files['b/d.php'] = '<?php echo "This is b/d.php\n"; ?>';
$files['e.php']   = '<?php echo "This is e.php\n"; ?>';

$manifest = '';
foreach($files as $name => $cont) {
	$len = strlen($cont);
	$manifest .= pack('V', strlen($name)) . $name . pack('VVVVC', $len, mktime (12, 0, 0, 3, 1, 2006), $len, crc32($cont), 0x00);
}
$alias = 'alias';
$manifest = pack('VnV', count($files), 0x0800, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= $cont;
}

file_put_contents($fname, $file);

?>