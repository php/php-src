--TEST--
Test the basics to function date_timestamp_get().
--CREDITS--
Carlos Alexandre Pires de Carvalho Junior <calexandrepcjr [at] gmail [dot] com>
Virgílio de Carvalho Pontes <vigiliocpontes [at] outlook [dot] com>
Vitor Mattos <vitor [at] php [dot] rio>
Henrique Ramos <henrique [at] habilissimo [dot] com [dot] br>
--FILE--
<?php
$beginDtObj = date_create('1970-01-01T00:00:00UTC');

$beginTimestamp = date_timestamp_get($beginDtObj);

var_dump($beginTimestamp);
?>
--EXPECT--
int(0)
