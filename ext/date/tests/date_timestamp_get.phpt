--TEST--
Test the basics to function date_timestamp_get().
--CREDITS--
Carlos Alexandre Pires de Carvalho Junior <calexandrepcjr [at] gmail [dot] com>
Daiane Alves <daiane [at] lt [dot] coop [dot] br>
Ednaldo Neimeg Marques <neimeg [at] gmail [dot] com>
Henrique Ramos <henrique [at] habilissimo [dot] com [dot] br>
Larissa Mourullo <larismourullo [at] gmail [dot] com>
Vinicius Dias <carlosv775 [at] gmail [dot] com>
Virgílio de Carvalho Pontes <vigiliocpontes [at] outlook [dot] com>
Vitor Mattos <vitor [at] php [dot] rio>
--FILE--
<?php
$beginDtObj = date_create('1970-01-01T00:00:00UTC');
$beginTimestamp = date_timestamp_get($beginDtObj);
var_dump($beginTimestamp);

// Test the DateTime feature alias in function date_timestamp_get().
$dateTimeTz = (new DateTime('1970-01-01T00:00:00UTC'))->getTimeStamp();
var_dump($dateTimeTz === $beginTimestamp);
?>
--EXPECT--
int(0)
bool(true)
