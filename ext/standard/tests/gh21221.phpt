--TEST--
GH-21221: Prevent closing of innerstream of php://temp stream
--CREDITS--
chongwick
--FILE--
<?php

$f = fopen('php://temp', 'r+b');
$resources = get_resources();
fclose(end($resources));

?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
