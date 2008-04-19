<?php
include dirname(__FILE__) . '/corrupt_zipmaker.php.inc';
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii');
$a->addFile('hi2', null, 'hii2', null, null, 'encrypt', 'encrypt');
$a->writeZip(dirname(__FILE__) . '/encrypted.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii');
$a->addFile('', null, 'stdin');
$a->writeZip(dirname(__FILE__) . '/stdin.zip');
$a = new corrupt_zipmaker;
$a->addFile('hii', null, 'hii', null, null, 'filename_len', 'filename_len');
$a->addFile('hi', null, 'hii');
$a->writeZip(dirname(__FILE__) . '/truncfilename.zip');
?>
