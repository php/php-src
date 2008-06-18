--TEST--
SPL: SplFileObject::fgetss (bug 45216)
--CREDITS--
Perrick Penet <perrick@noparking.net>
#testfest phpcampparis 2008-06-07
--FILE--
<?php 
$file = dirname(__FILE__) . '/foo.html';
file_put_contents($file, 'text 0<div class="tested">text 1</div>');
$handle = fopen($file, 'r');

$object = new SplFileObject($file);
var_dump($object->fgetss());
var_dump(fgetss($handle));
?>
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/foo.html');
?>
--EXPECTF--
string(12) "text 0text 1"
string(12) "text 0text 1"
