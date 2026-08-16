--TEST--
GH-23276 (ZipArchive subclass storing its own stream is collectable)
--EXTENSIONS--
zip
--FILE--
<?php
class Holder extends ZipArchive {
    public $stream;
    public $bag = [];
}

$filename = __DIR__ . '/gh23276.zip';

$zip = new Holder;
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('entry.txt', 'contents');
$zip->close();

// An archive holding its own stream in a property must be destroyed.
$zip->open($filename, ZipArchive::RDONLY);
$zip->stream = $zip->getStream('entry.txt');
$weakRef = WeakReference::create($zip);
unset($zip);
var_dump($weakRef->get());

// Same through an indirect edge (property -> array -> resource).
$zip = new Holder;
$zip->open($filename, ZipArchive::RDONLY);
$zip->bag[] = $zip->getStream('entry.txt');
$weakRef = WeakReference::create($zip);
unset($zip);
gc_collect_cycles();
var_dump($weakRef->get());

// Two archives cross-holding each other's streams.
$a = new Holder;
$b = new Holder;
$a->open($filename, ZipArchive::RDONLY);
$b->open($filename, ZipArchive::RDONLY);
$a->stream = $b->getStream('entry.txt');
$b->stream = $a->getStream('entry.txt');
$weakRef = WeakReference::create($a);
unset($a, $b);
gc_collect_cycles();
var_dump($weakRef->get());

// An externally held stream no longer keeps the object alive, but it must still keep the underlying archive readable.
$zip = new Holder;
$zip->open($filename, ZipArchive::RDONLY);
$stream = $zip->getStream('entry.txt');
$weakRef = WeakReference::create($zip);
unset($zip);
var_dump($weakRef->get());
var_dump(stream_get_contents($stream));
fclose($stream);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23276.zip');
?>
--EXPECT--
NULL
NULL
NULL
NULL
string(8) "contents"
