<?php
error_reporting(E_ALL);
$a = unserialize(file_get_contents('test.dat'));
var_dump($a);
$a = unserialize(file_get_contents('C:\\development\\pear-core\\tests\\PEAR_DependencyDB\\tester\\.registry\\archive_tar.reg'));
$fp = fopen('C:\\development\\pear-core\\tests\\PEAR_DependencyDB\\tester\\.registry\\archive_tar.reg', 'r');
$a = fread($fp, 4501);
fclose($fp);
var_dump(substr($a, 2420));
var_dump($a);
?>