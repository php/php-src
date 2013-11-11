<?php
$z = new ZipArchive;
$z->open('a.zip', ZIPARCHIVE::CREATE);

/* or 'remove_all_path' => 0*/
$options = array('remove_path' => '/home/pierre/cvs/gd/libgd/tests',
'add_path' => 'images/',
);

$found = $z->addPattern("/(\.png)$/i", "/home/pierre/cvs/gd/libgd/tests", $options);
var_dump($found);
$z->close();

