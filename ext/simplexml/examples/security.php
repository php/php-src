<?php
$s = simplexml_load_file('security.xml');
echo $s->id;
$s->id = 20;
simplexml_save_file($s, 'security.new.xml');
?>
