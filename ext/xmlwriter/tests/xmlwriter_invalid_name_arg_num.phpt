--TEST--
XMLWriter: an invalid name reports the argument number of the call style used
--EXTENSIONS--
xmlwriter
--FILE--
<?php
$bad = 'bad name';

$methods = [
    ['startAttribute', [$bad]],
    ['writeAttribute', [$bad, 'value']],
    ['startAttributeNs', ['p', $bad, 'urn:x']],
    ['writeAttributeNs', ['p', $bad, 'urn:x', 'value']],
    ['startElement', [$bad]],
    ['writeElement', [$bad, 'content']],
    ['startElementNs', ['p', $bad, 'urn:x']],
    ['writeElementNs', ['p', $bad, 'urn:x', 'content']],
    ['startPi', [$bad]],
    ['writePi', [$bad, 'content']],
    ['startDtdElement', [$bad]],
    ['writeDtdElement', [$bad, 'content']],
    ['startDtdAttlist', [$bad]],
    ['writeDtdAttlist', [$bad, 'content']],
    ['startDtdEntity', [$bad, false]],
    ['writeDtdEntity', [$bad, 'content']],
];

echo "-- method calls --\n";
foreach ($methods as [$method, $args]) {
    $writer = new XMLWriter();
    $writer->openMemory();
    try {
        $writer->$method(...$args);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "\n-- procedural calls --\n";
foreach ($methods as [$method, $args]) {
    $function = 'xmlwriter_' . strtolower(preg_replace('/(?<!^)[A-Z]/', '_$0', $method));
    $writer = xmlwriter_open_memory();
    try {
        $function($writer, ...$args);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
-- method calls --
XMLWriter::startAttribute(): Argument #1 ($name) must be a valid attribute name, "bad name" given
XMLWriter::writeAttribute(): Argument #1 ($name) must be a valid attribute name, "bad name" given
XMLWriter::startAttributeNs(): Argument #2 ($name) must be a valid attribute name, "bad name" given
XMLWriter::writeAttributeNs(): Argument #2 ($name) must be a valid attribute name, "bad name" given
XMLWriter::startElement(): Argument #1 ($name) must be a valid element name, "bad name" given
XMLWriter::writeElement(): Argument #1 ($name) must be a valid element name, "bad name" given
XMLWriter::startElementNs(): Argument #2 ($name) must be a valid element name, "bad name" given
XMLWriter::writeElementNs(): Argument #2 ($name) must be a valid element name, "bad name" given
XMLWriter::startPi(): Argument #1 ($target) must be a valid PI target, "bad name" given
XMLWriter::writePi(): Argument #1 ($target) must be a valid PI target, "bad name" given
XMLWriter::startDtdElement(): Argument #1 ($qualifiedName) must be a valid element name, "bad name" given
XMLWriter::writeDtdElement(): Argument #1 ($name) must be a valid element name, "bad name" given
XMLWriter::startDtdAttlist(): Argument #1 ($name) must be a valid element name, "bad name" given
XMLWriter::writeDtdAttlist(): Argument #1 ($name) must be a valid element name, "bad name" given
XMLWriter::startDtdEntity(): Argument #1 ($name) must be a valid attribute name, "bad name" given
XMLWriter::writeDtdEntity(): Argument #1 ($name) must be a valid element name, "bad name" given

-- procedural calls --
xmlwriter_start_attribute(): Argument #2 ($name) must be a valid attribute name, "bad name" given
xmlwriter_write_attribute(): Argument #2 ($name) must be a valid attribute name, "bad name" given
xmlwriter_start_attribute_ns(): Argument #3 ($name) must be a valid attribute name, "bad name" given
xmlwriter_write_attribute_ns(): Argument #3 ($name) must be a valid attribute name, "bad name" given
xmlwriter_start_element(): Argument #2 ($name) must be a valid element name, "bad name" given
xmlwriter_write_element(): Argument #2 ($name) must be a valid element name, "bad name" given
xmlwriter_start_element_ns(): Argument #3 ($name) must be a valid element name, "bad name" given
xmlwriter_write_element_ns(): Argument #3 ($name) must be a valid element name, "bad name" given
xmlwriter_start_pi(): Argument #2 ($target) must be a valid PI target, "bad name" given
xmlwriter_write_pi(): Argument #2 ($target) must be a valid PI target, "bad name" given
xmlwriter_start_dtd_element(): Argument #2 ($qualifiedName) must be a valid element name, "bad name" given
xmlwriter_write_dtd_element(): Argument #2 ($name) must be a valid element name, "bad name" given
xmlwriter_start_dtd_attlist(): Argument #2 ($name) must be a valid element name, "bad name" given
xmlwriter_write_dtd_attlist(): Argument #2 ($name) must be a valid element name, "bad name" given
xmlwriter_start_dtd_entity(): Argument #2 ($name) must be a valid attribute name, "bad name" given
xmlwriter_write_dtd_entity(): Argument #2 ($name) must be a valid element name, "bad name" given
