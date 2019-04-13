--TEST--
Test the DateTime feature alias in function date_timestamp_get().
--CREDITS--
Carlos Alexandre Pires de Carvalho Junior <calexandrepcjr [at] gmail [dot] com>
Daiane Alves <daiane [at] lt [dot] coop [dot] br>
Ednaldo Neimeg Marques <neimeg [at] gmail [dot] com>
Henrique Ramos <henrique [at] habilissimo [dot] com [dot] br>
Larissa Mourullo <larismourullo [at] gmail [dot] com>
--FILE--
<?php
$dateTimeTz = (new DateTime('1970-01-01T00:00:00UTC'))->getTimeStamp();

$beginDtObj = date_create('1970-01-01T00:00:00UTC');
$beginTimestamp = date_timestamp_get($beginDtObj);

var_dump($dateTimeTz === $beginTimestamp);
?>
--EXPECT--
bool(true)
