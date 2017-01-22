--TEST--
Bug #70748 (Segfault in ini_lex () at Zend/zend_ini_scanner.l)
--FILE--
<?php
$ini = '[${ 	';

$ini_file = __DIR__ . "/bug70748.ini";

file_put_contents($ini_file, $ini);

var_dump(parse_ini_file($ini_file));
?>
--CLEAN--
<?php
unlink(__DIR__ . "/bug70748.ini");
?>
--EXPECTF--
Warning: syntax error, unexpected $end, expecting '}' in %sbug70748.ini on line %d
 in %sbug70748.php on line %d
bool(false)
