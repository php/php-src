--TEST--
GH-16814 (MessageFormatter::format throws error on unused parameter)
--EXTENSIONS--
intl
--FILE--
<?php
$fmt = new MessageFormatter('de', 'Beispiel ohne Benutzung');
$fmt->format(['payed' => new DateTimeImmutable()]);
echo "OK";
?>
--EXPECT--
OK
