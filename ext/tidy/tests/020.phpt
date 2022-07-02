--TEST--
OO API
--EXTENSIONS--
tidy
--FILE--
<?php

$tidy = new tidy();
$str  = <<<EOF
<p>Isto é um texto em Português<br>
para testes.</p>
EOF;

$tidy->parseString($str, array('output-xhtml'=>1), 'latin1');
$tidy->cleanRepair();
$tidy->diagnose();
var_dump(tidy_warning_count($tidy) > 0);
var_dump(strlen($tidy->errorBuffer) > 50);

echo $tidy;
?>
--EXPECTF--
bool(true)
bool(true)
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html%A>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title></title>
</head>
<body>
<p>Isto é um texto em Português<br />
para testes.</p>
</body>
</html>
