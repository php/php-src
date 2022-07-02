--TEST--
XMLReader: libxml2 XML Reader, setRelaxNGSchema
--EXTENSIONS--
xmlreader
--FILE--
<?php

$xmlstring = '<TEI.2>hello</TEI.2>';
$relaxngfile = __DIR__ . '/relaxNG.rng';
$file = __DIR__ . '/_007.xml';
file_put_contents($file, $xmlstring);

$reader = new XMLReader();
$reader->open($file);

if ($reader->setRelaxNGSchema($relaxngfile)) {
  while ($reader->read());
}
if ($reader->isValid()) {
  print "file relaxNG: ok\n";
} else {
  print "file relaxNG: failed\n";
}
$reader->close();
unlink($file);


$reader = new XMLReader();
$reader->XML($xmlstring);

if ($reader->setRelaxNGSchema($relaxngfile)) {
  while ($reader->read());
}
if ($reader->isValid()) {
  print "string relaxNG: ok\n";
} else {
  print "string relaxNG: failed\n";
}

$reader->close();

$reader = new XMLReader();
$reader->XML($xmlstring);

try {
    $reader->setRelaxNGSchema('');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$reader->close();
?>
--EXPECT--
file relaxNG: ok
string relaxNG: ok
XMLReader::setRelaxNGSchema(): Argument #1 ($filename) cannot be empty
