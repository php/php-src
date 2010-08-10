--TEST--
Bug #52573 (SplFileObject::fscanf Segmentation fault)
--FILE--
<?php

$result = null;
$f = new SplFileObject(__FILE__, 'r');
$f->fscanf('<?php // %s', $result);

?>
--EXPECTF--
Warning: Parameter 3 to fscanf() expected to be a reference, value given in %s on line 5
