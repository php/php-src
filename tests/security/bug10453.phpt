--TEST--
Bug #10453 (using a high amount of filters for nefarious purposes)
--FILE--
<?php
$fp = fopen('php://output', 'w');
for($i=0; $i<10; $i++)
    stream_filter_append($fp, 'string.rot13');
fwrite($fp, "This is a test.\n");

$fp = fopen('php://filter/write=string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13|string.rot13/resource=php://output', 'w');
fwrite($fp, "This is a test.\n");
?>
--EXPECTF--
This is a test.

Fatal error: Uncaught Exception: Unable to apply filter, maximum number (5) reached in %s
Stack trace:
#0 %s: fopen('php://filter/wr...', 'w')
#1 {main}
  thrown in %s
