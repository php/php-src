<?php
$s = simplexml_load_file('security.xml');
echo $s->id;
$s->id = 20;
simplexml_save_document_file('security.new.xml', $s);
?>
