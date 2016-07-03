<?php

$z = new ZipArchive;
$z->open('a.zip', ZIPARCHIVE::CREATE);

/* or 'remove_all_path' => 0*/
$options = array(
	'remove_path' => '/home/francis/myimages',
	'add_path' => 'images/',
);
$found = $z->addGlob("/home/pierre/cvs/gd/libgd/tests/*.png", 0, $options);
var_dump($found);
$z->close();

