--TEST--
GH-8996: DOMNode serialization on PHP ^8.1
--EXTENSIONS--
dom
--FILE--
<?php

echo "=== __sleep and __wakeup ===\n";

class SerializableDomDocumentSleepWakeup extends DOMDocument
{
    private $xmlData;

    public function __sleep(): array
    {
        $this->xmlData = $this->saveXML();
        return ['xmlData'];
    }

    public function __wakeup(): void
    {
        $this->loadXML($this->xmlData);
    }
}

$dom = new SerializableDomDocumentSleepWakeup('1.0', 'UTF-8');
$dom->loadXML('<tag>value</tag>');

$serialized = serialize($dom);
var_dump($serialized);
$unserialized = unserialize($serialized);

echo "Serialized:\n-----------\n$serialized\n-----------\nRestored:\n-----------\n{$unserialized->saveXml()}";

echo "=== __serialize and __unserialize ===\n";

class SerializableDomDocument__Serialize__Unserialize extends DOMDocument
{
    public function __serialize(): array
    {
        return ['xmlData' => $this->saveXML()];
    }

    public function __unserialize(array $data): void
    {
        $this->loadXML($data['xmlData']);
    }
}

$dom = new SerializableDomDocument__Serialize__Unserialize('1.0', 'UTF-8');
$dom->loadXML('<tag>value</tag>');

$serialized = serialize($dom);
$unserialized = unserialize($serialized);

echo "Serialized:\n-----------\n$serialized\n-----------\nRestored:\n-----------\n{$unserialized->saveXml()}";

echo "=== serialize and unserialize ===\n";

class SerializableDomDocumentSerializeUnserialize extends DOMDocument implements Serializable
{
    public function serialize(): ?string
    {
        return $this->saveXML();
    }

    public function unserialize(string $data): void
    {
        $this->loadXML($data);
    }
}

$dom = new SerializableDomDocumentSerializeUnserialize('1.0', 'UTF-8');
$dom->loadXML('<tag>value</tag>');

$serialized = serialize($dom);
$unserialized = unserialize($serialized);

echo "Serialized:\n-----------\n$serialized\n-----------\nRestored:\n-----------\n{$unserialized->saveXml()}";

?>
--EXPECTF--
=== __sleep and __wakeup ===
string(144) "O:34:"SerializableDomDocumentSleepWakeup":1:{s:43:"%0SerializableDomDocumentSleepWakeup%0xmlData";s:39:"<?xml version="1.0"?>
<tag>value</tag>
";}"
Serialized:
-----------
O:34:"SerializableDomDocumentSleepWakeup":1:{s:43:"%0SerializableDomDocumentSleepWakeup%0xmlData";s:39:"<?xml version="1.0"?>
<tag>value</tag>
";}
-----------
Restored:
-----------
<?xml version="1.0"?>
<tag>value</tag>
=== __serialize and __unserialize ===
Serialized:
-----------
O:47:"SerializableDomDocument__Serialize__Unserialize":1:{s:7:"xmlData";s:39:"<?xml version="1.0"?>
<tag>value</tag>
";}
-----------
Restored:
-----------
<?xml version="1.0"?>
<tag>value</tag>
=== serialize and unserialize ===

Deprecated: SerializableDomDocumentSerializeUnserialize implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
Serialized:
-----------
C:43:"SerializableDomDocumentSerializeUnserialize":39:{<?xml version="1.0"?>
<tag>value</tag>
}
-----------
Restored:
-----------
<?xml version="1.0"?>
<tag>value</tag>
