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

class ResurrectingHolder extends Holder {
    public function __destruct() {
        $GLOBALS['resurrected'] = $this;
    }
}

function getEntryStream(ZipArchive $zip) {
    $stream = $zip->getStream('entry.txt');
    if (!is_resource($stream)) {
        throw new Exception('Failed to open entry stream');
    }
    return $stream;
}

$filename = __DIR__ . '/gh23276.zip';

$zip = new Holder;
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('entry.txt', 'contents');
$zip->close();

// An archive holding its own stream in a property must be destroyed.
$zip->open($filename, ZipArchive::RDONLY);
$zip->stream = getEntryStream($zip);
$weakRef = WeakReference::create($zip);
unset($zip);
var_dump($weakRef->get());

// Same through an indirect edge (property -> array -> resource).
$zip = new Holder;
$zip->open($filename, ZipArchive::RDONLY);
$zip->bag[] = getEntryStream($zip);
$weakRef = WeakReference::create($zip);
unset($zip);
gc_collect_cycles();
var_dump($weakRef->get());

// Two archives cross-holding each other's streams.
$a = new Holder;
$b = new Holder;
$a->open($filename, ZipArchive::RDONLY);
$b->open($filename, ZipArchive::RDONLY);
$a->stream = getEntryStream($b);
$b->stream = getEntryStream($a);
$weakRef = WeakReference::create($a);
unset($a, $b);
gc_collect_cycles();
var_dump($weakRef->get());

// A resurrected object must retain a usable stream.
$zip = new ResurrectingHolder;
$zip->open($filename, ZipArchive::RDONLY);
$zip->stream = getEntryStream($zip);
unset($zip);
gc_collect_cycles();
var_dump($resurrected instanceof ResurrectingHolder);
var_dump(stream_get_contents($resurrected->stream));
fclose($resurrected->stream);
unset($resurrected);

// Externally held streams no longer keep the object alive. Closing one stream
// must not close the archive while another stream still uses it.
$zip = new Holder;
$zip->open($filename, ZipArchive::RDONLY);
$stream1 = getEntryStream($zip);
$stream2 = getEntryStream($zip);
$weakRef = WeakReference::create($zip);
unset($zip);
var_dump($weakRef->get());
fclose($stream1);
var_dump(stream_get_contents($stream2));
fclose($stream2);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23276.zip');
?>
--EXPECT--
NULL
NULL
NULL
bool(true)
string(8) "contents"
NULL
string(8) "contents"
