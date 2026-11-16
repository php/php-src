--TEST--
XMLReader::getAttributeNo() doesn't reach hard limit
--EXTENSIONS--
xmlreader
--INI--
memory_limit=2M
--FILE--
<?php
$path = __DIR__ . '/' . basename(__FILE__, '.php') . '.tmp';
$fp = fopen($path, 'wb');
/* A single 6 MB attribute value, written in chunks so building the
 * document stays inside the soft limit. */
$chunk = str_repeat("x", 512 * 1024);
fwrite($fp, "<r a=\"");
for ($i = 0; $i < 12; $i++) {
    fwrite($fp, $chunk);
}
fwrite($fp, "\"/>");
unset($chunk);
fclose($fp);

$reader = XMLReader::open($path);
$reader->read();
$reader->getAttributeNo(0);
echo "not reached\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tmp');
?>
--EXPECTF--
Fatal error: Allowed memory size of 2097152 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
