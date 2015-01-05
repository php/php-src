<?php
include dirname(__FILE__) . '/corrupt_zipmaker.php.inc';
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii', null, null, null, 'extralen1');
$a->writeZip(dirname(__FILE__) . '/extralen_toolong.zip');
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii');
$a->writeZip(dirname(__FILE__) . '/disknumber.zip', 'disknumber');
?>
