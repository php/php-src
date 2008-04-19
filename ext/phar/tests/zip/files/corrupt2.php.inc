<?php
include dirname(__FILE__) . '/corrupt_zipmaker.php.inc';
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii');
$a->addFile('hi2', null, 'hii2', null, null, 'encrypt', 'encrypt');
$a->writeZip(dirname(__FILE__) . '/encrypted.zip');
?>
