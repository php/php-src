<?php

function xmlwriter_open_uri(string $uri): XMLWriter|false {}

function xmlwriter_open_memory(): XMLWriter|false {}

function xmlwriter_set_indent(XMLWriter $xmlwriter, bool $indent): bool {}

function xmlwriter_set_indent_string(XMLWriter $xmlwriter, string $indentString): bool {}

function xmlwriter_start_comment(XMLWriter $xmlwriter): bool {}

function xmlwriter_end_comment(XMLWriter $xmlwriter): bool {}

function xmlwriter_start_attribute(XMLWriter $xmlwriter, string $name): bool {}

function xmlwriter_end_attribute(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_attribute(XMLWriter $xmlwriter, string $name, string $value): bool {}

function xmlwriter_start_attribute_ns(XMLWriter $xmlwriter, string $prefix, string $name, ?string $uri): bool {}

function xmlwriter_write_attribute_ns(XMLWriter $xmlwriter, string $prefix, string $name, ?string $uri, string $content): bool {}

function xmlwriter_start_element(XMLWriter $xmlwriter, string $name): bool {}

function xmlwriter_end_element(XMLWriter $xmlwriter): bool {}

function xmlwriter_full_end_element(XMLWriter $xmlwriter): bool {}

function xmlwriter_start_element_ns(XMLWriter $xmlwriter, ?string $prefix, string $name, ?string $uri): bool {}

function xmlwriter_write_element(XMLWriter $xmlwriter, string $name, ?string $content = null): bool {}

function xmlwriter_write_element_ns(XMLWriter $xmlwriter, ?string $prefix, string $name, ?string $uri, ?string $content = null): bool {}

function xmlwriter_start_pi(XMLWriter $xmlwriter, string $target): bool {}

function xmlwriter_end_pi(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_pi(XMLWriter $xmlwriter, string $target, string $content): bool {}

function xmlwriter_start_cdata(XMLWriter $xmlwriter): bool {}

function xmlwriter_end_cdata(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_cdata(XMLWriter $xmlwriter, string $content): bool {}

function xmlwriter_text(XMLWriter $xmlwriter, string $content): bool {}

function xmlwriter_write_raw(XMLWriter $xmlwriter, string $content): bool {}

function xmlwriter_start_document(XMLWriter $xmlwriter, ?string $version = '1.0', ?string $encoding = null, ?string $standalone = null): bool {}

function xmlwriter_end_document(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_comment(XMLWriter $xmlwriter, string $content): bool {}

function xmlwriter_start_dtd(XMLWriter $xmlwriter, string $qualifiedName, ?string $publicId = null, ?string $systemId = null): bool {}

function xmlwriter_end_dtd(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_dtd(XMLWriter $xmlwriter, string $name, ?string $publicId = null, ?string $systemId = null, ?string $subset = null): bool {}

function xmlwriter_start_dtd_element(XMLWriter $xmlwriter, string $qualifiedName): bool {}

function xmlwriter_end_dtd_element(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_dtd_element(XMLWriter $xmlwriter, string $name, string $content): bool {}

function xmlwriter_start_dtd_attlist(XMLWriter $xmlwriter, string $name): bool {}

function xmlwriter_end_dtd_attlist(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_dtd_attlist(XMLWriter $xmlwriter, string $name, string $content): bool {}

function xmlwriter_start_dtd_entity(XMLWriter $xmlwriter, string $name, bool $isparam): bool {}

function xmlwriter_end_dtd_entity(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_dtd_entity(XMLWriter $xmlwriter, string $name, string $content, bool $isparam = false, string $publicId = UNKNOWN, string $systemId = UNKNOWN, string $ndataid = UNKNOWN): bool {}

function xmlwriter_output_memory(XMLWriter $xmlwriter, bool $flush = true): string {}

function xmlwriter_flush(XMLWriter $xmlwriter, bool $empty = true): string|int {}

class XMLWriter
{
    public function openUri(string $uri): bool {}

    public function openMemory(): bool {}

    public function setIndent(bool $indent): bool {}

    public function setIdentString(string $indentString): bool {}

    public function startComment(): bool {}

    public function endComment(): bool {}

    public function startAttribute(string $name): bool {}

    public function endAttribute(): bool {}

    public function writeAttribute(string $name, string $value): bool {}

    public function startAttributeNs(string $prefix, string $name, ?string $uri): bool {}

    public function writeAttributeNs(string $prefix, string $name, ?string $uri, string $content): bool {}

    public function startElement(string $name): bool {}

    public function endElement(): bool {}

    public function fullEndElement(): bool {}

    public function startElementNs(?string $prefix, string $name, ?string $uri): bool {}

    public function writeElement(string $name, ?string $content = null): bool {}

    public function writeElementNs(?string $prefix, string $name, ?string $uri, ?string $content = null): bool {}

    public function startPi(string $target): bool {}

    public function endPi(): bool {}

    public function writePi(string $target, string $content): bool {}

    public function startCdata(): bool {}

    public function endCdata(): bool {}

    public function writeCdata(string $content): bool {}

    public function text(string $content): bool {}

    public function writeRaw(string $content): bool {}

    public function startDocument(?string $version = '1.0', ?string $encoding = null, ?string $standalone = null): bool {}

    public function endDocument(): bool {}

    public function writeComment(string $content): bool {}

    public function startDtd(string $qualifiedName, ?string $publicId = null, ?string $systemId = null): bool {}

    public function endDtd(): bool {}

    public function writeDtd(string $name, ?string $publicId = null, ?string $systemId = null, ?string $subset = null): bool {}

    public function startDtdElement(string $qualifiedName): bool {}

    public function endDtdElement(): bool {}

    public function writeDtdElement(string $name, string $content): bool {}

    public function startDtdAttlist(string $name): bool {}

    public function endDtdAttlist(): bool {}

    public function writeDtdAttlist(string $name, string $content): bool {}

    public function startDtdEntity(string $name, bool $isparam): bool {}

    public function endDtdEntity(): bool {}

    public function writeDtdEntity(string $name, string $content, bool $isparam, string $publicId = UNKNOWN, string $systemId = UNKNOWN, string $ndataid = UNKNOWN): bool {}

    public function outputMemory(bool $flush = true): string {}

    public function flush(bool $empty = true): string|int {}
}
