<?php
include dirname(__FILE__) . '/corrupt_zipmaker.php.inc';
$a = new corrupt_zipmaker;
$a->addFile('hi', null, 'hii');
$a->addFile('hi2', null, 'hii2');
$a->writeZip(dirname(__FILE__) . '/count1.zip', 'count1');
$a->writeZip(dirname(__FILE__) . '/count2.zip', 'count2');
$a->writeZip(dirname(__FILE__) . '/nozipend.zip', 'none');
$a->writeZip(dirname(__FILE__) . '/filecomment.zip', 'comment');
$a->writeZip(dirname(__FILE__) . '/cdir_offset.zip', 'cdir_offset');
?>
