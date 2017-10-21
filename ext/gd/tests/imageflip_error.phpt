--TEST--
Test invalid parameters in imageflip
--CREDIT--
PHP TestFest 2017 - PHPRio
Ahmed Yarub Hani <ahmedyarub@yahoo.com>
Camila Lacerda <milacerda@gmail.com>
Daiane Alves <dsalves@id.uff.br>
Fernando Soares <fsoares.qa@gmail.com>
Lucas Shelby <soares-lucas94@hotmail.com>
Ruan Azevedo <ruan.azevedo@gmail.com>
Thiago Alves <thiago9912@gmail.com>
Virgílio C Pontes <virgiliocpontes@outlook.com>
Vitor Mattos <vitor.mattos@phprio.org>
--FILE--
<?php

$im = imagecreatetruecolor( 99, 99 );

imagesetpixel($im, 0, 0, 0xFF);
imagesetpixel($im, 0, 98, 0x00FF00);
imagesetpixel($im, 98, 0, 0xFF0000);
imagesetpixel($im, 98, 98, 0x0000FF);

$result=imageflip($im, 4);
imageflip(NULL, IMG_FLIP_BOTH);
var_dump($result);
var_dump(imageflip($im,'a'));
?>
--EXPECTF--
Warning: imageflip(): Unknown flip mode in %s on line %d

Warning: imageflip() expects parameter 1 to be resource, null given in %s on line %d
bool(false)

Warning: imageflip() expects parameter 2 to be integer, string given in %s on line %d
NULL

