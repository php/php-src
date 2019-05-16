--TEST--
PHP bug #77143: Heap Buffer Overflow (READ: 4) in phar_parse_pharfile
--INI--
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
chdir(__DIR__);
try {
var_dump(new Phar('bug77143.phar',0,'project.phar'));
echo "OK\n";
} catch(UnexpectedValueException $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
internal corruption of phar "%sbug77143.phar" (truncated manifest header)
