--TEST--
OO API
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
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
--EXPECT--
bool(true)
bool(true)
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title></title>
</head>
<body>
<p>Isto é um texto em Português<br />
para testes.</p>
</body>
</html>
