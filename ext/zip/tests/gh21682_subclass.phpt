--TEST--
GH-21682 (ZipArchive subclass implements serialization via __serialize()/__unserialize())
--EXTENSIONS--
zip
--FILE--
<?php
class MyArchive extends ZipArchive
{
    public function __serialize(): array
    {
        return ['data' => $this->closeString()];
    }

    public function __unserialize(array $data): void
    {
        $this->openString($data['data']);
    }
}

$zip = new MyArchive();
$zip->openString();
$zip->addFromString('test1', 'abc123');

$roundtrip = unserialize(serialize($zip));
var_dump($roundtrip instanceof MyArchive);
var_dump($roundtrip->numFiles);
var_dump($roundtrip->getFromName('test1'));
?>
--EXPECT--
bool(true)
int(1)
string(6) "abc123"
