--TEST--
Bug #67805 SplFileObject setMaxLineLength
--CREDITS--
Willian Gustavo Veiga <contact@willianveiga.com>
--FILE--
<?php
$splFileObject = new SplFileObject(__FILE__);
$splFileObject->setMaxLineLen(3);
$line = $splFileObject->getCurrentLine();
var_dump($line === '<?p');
var_dump(strlen($line) === 3);
?>
--EXPECTF--
bool(true)
bool(true)
