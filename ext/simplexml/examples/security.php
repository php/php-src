<?php
$s = simplexml_load_file('security.xml');
echo $s->id;
$s->id = 20;
$s->to_xml_file('security.new.xml');
?>
