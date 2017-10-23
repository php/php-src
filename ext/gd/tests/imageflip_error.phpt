--TEST--
Test invalid parameters in imageflip
--CREDIT--
PHP TestFest 2017 - PHPRio
Ahmed Yarub Hani <ahmedyarub@yahoo.com>
Camila Lacerda <milacerda@gmail.com>
Daiane Alves <dsalves@id.uff.br>
Fernando Soares <fsoares.qa@gmail.com>
Lucas Shelby <soares-lucas94@hotmail.com>
Raphael Almeida <jaguarnet7@gmail.com>
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

var_dump(imageflip(NULL, IMG_FLIP_BOTH));
var_dump(imageflip($im, 'invalid_mode'));
var_dump(imageflip(fopen(__FILE__, 'r'), IMG_FLIP_BOTH));
define('IMG_FLIP_INVALID', 9999);
var_dump(imageflip($im, IMG_FLIP_INVALID));
?>
--EXPECTF--
Warning: imageflip() expects parameter 1 to be resource, null given in %s on line %d
NULL

Warning: imageflip() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: imageflip(): supplied resource is not a valid Image resource in %s on line %d
bool(false)

Warning: imageflip(): Unknown flip mode in %s on line %d
bool(false)

