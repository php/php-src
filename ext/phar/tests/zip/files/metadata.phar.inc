<?php
@unlink(dirname(__FILE__) . '/metadata.phar.zip');
$a = new Phar(dirname(__FILE__) . '/metadata.phar.zip');
$a['a'] = 'a';
$a['b'] = 'b';
$a['b']->setMetadata('hi there');
$a['c'] = 'c';
$a['c']->setMetadata(array('hi', 'there'));
$a['d'] = 'd';
$a['d']->setMetadata(array('hi'=>'there','foo'=>'bar'));
?>
