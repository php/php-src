<?php

$filename="";

$bb = is_file($filename);
var_dump($bb);

$aa = file_exists($filename);
var_dump($aa);

$filename="php.ini-dist";

$bb = is_file($filename);
var_dump($bb);

$aa = file_exists($filename);
var_dump($aa);

?>
