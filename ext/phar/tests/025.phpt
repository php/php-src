--TEST--
Phar: phar:// require (repeated names)
--INI--
magic_quotes_runtime=0
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
$file = "<?php __HALT_COMPILER(); ?>";
$a = '<?php echo "This is a\n"; ?>';
$b = '<?php echo "This is b\n"; ?>';
$bb= '<?php echo "This is b/b\n"; ?>';

$manifest = '';
$manifest .= pack('V', 1) . 'a' .   pack('VVVV', strlen($a),  time(),                  0, strlen($a)  + 8);
$manifest .= pack('V', 1) . 'b' .   pack('VVVV', strlen($b),  time(), strlen($a)    +  8, strlen($b)  + 8);
$manifest .= pack('V', 3) . 'b/b' . pack('VVVV', strlen($bb), time(), strlen($a.$b) + 16, strlen($bb) + 8);
$file .= pack('VV', strlen($manifest) + 4, 3) .
	 $manifest .
	 pack('VV', crc32($a),  strlen($a)) . $a .
	 pack('VV', crc32($b),  strlen($b)) . $b .
	 pack('VV', crc32($bb), strlen($bb)) . $bb;

file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);

include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/a';
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b';
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b/b';

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
This is a
This is b
This is b/b
===DONE===
