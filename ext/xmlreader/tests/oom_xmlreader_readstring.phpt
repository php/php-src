--TEST--
XMLReader::readString() doesn't reach hard limit
--EXTENSIONS--
xmlreader
--INI--
memory_limit=2M
--SKIPIF--
<?php
if (getenv('SKIP_ASAN') && LIBXML_VERSION < 21000) die('skip xmlTextReaderReadString() in versions <2.10.0 is slow under ASan');
?>
--FILE--
<?php
$path = __DIR__ . '/' . basename(__FILE__, '.php') . '.tmp';
$fp = fopen($path, 'wb');
/* ~11 MB of small elements: no single node is huge, but the whole
 * subtree is, and it is serialized into one Zend string. */
$piece = str_repeat("<a>xxxxxxxx</a>", 32768);
fwrite($fp, "<r>");
for ($i = 0; $i < 12; $i++) {
    fwrite($fp, $piece);
}
fwrite($fp, "</r>");
unset($piece);
fclose($fp);

$reader = XMLReader::open($path);
$reader->read();
$reader->readString();
echo "not reached\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tmp');
?>
--EXPECTF--
Fatal error: Allowed memory size of 2097152 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
