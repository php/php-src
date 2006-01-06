--TEST--
Phar: phar:// require
--INI--
magic_quotes_runtime=0
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
$file = "<?php __HALT_COMPILER(); ?>";
$a = '<?php echo "This is a\n"; ?>';
$b = '<?php echo "This is b\n"; ?>';
$bb= '<?php echo "This is b/c\n"; ?>';

$manifest = '';
$manifest .= pack('V', 5) . 'a.php' .   pack('VVVV', strlen($a),  time(),                  0, strlen($a)  + 8);
$manifest .= pack('V', 5) . 'b.php' .   pack('VVVV', strlen($b),  time(), strlen($a)    +  8, strlen($b)  + 8);
$manifest .= pack('V', 7) . 'b/c.php' . pack('VVVV', strlen($bb), time(), strlen($a.$b) + 16, strlen($bb) + 8);
$file .= pack('VV', strlen($manifest) + 4, 3) .
	 $manifest .
	 pack('VV', crc32($a),  strlen($a)) . $a .
	 pack('VV', crc32($b),  strlen($b)) . $b .
	 pack('VV', crc32($bb), strlen($bb)) . $bb;

file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);

include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/a.php';
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b.php';
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b/c.php';

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
This is a
This is b
This is b/c
===DONE===
