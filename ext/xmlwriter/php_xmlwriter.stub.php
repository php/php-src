<?php

/** @generate-function-entries */

function xmlwriter_open_uri(string $uri): XMLWriter|false {}

function xmlwriter_open_memory(): XMLWriter|false {}

function xmlwriter_set_indent(XMLWriter $xmlwriter, bool $indent): bool {}

function xmlwriter_set_indent_string(XMLWriter $xmlwriter, string $indentString): bool {}

function xmlwriter_start_comment(XMLWriter $xmlwriter): bool {}

function xmlwriter_end_comment(XMLWriter $xmlwriter): bool {}

function xmlwriter_start_attribute(XMLWriter $xmlwriter, string $name): bool {}

function xmlwriter_end_attribute(XMLWriter $xmlwriter): bool {}

function xmlwriter_write_attribute(XMLWriter $xmlwriter, string $name, string $value): bool {}

function xmlwriter_start_attribute_ns(XMLWriter $xmlwriter, ?string $prefix, string $name, ?string $uri): bool {}

function xmlwriter_write_attribute_ns(XMLWriter $xmlwriter, ?string $prefix, string $name, ?string $uri, string $content): bool {}

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
    /** @alias xmlwriter_open_uri */
    public function openUri(string $uri): bool {}

    /** @alias xmlwriter_open_memory */
    public function openMemory(): bool {}

    /** @alias xmlwriter_set_indent */
    public function setIndent(bool $indent): bool {}

    /** @alias xmlwriter_set_indent_string */
    public function setIndentString(string $indentString): bool {}

    /** @alias xmlwriter_start_comment */
    public function startComment(): bool {}

    /** @alias xmlwriter_end_comment */
    public function endComment(): bool {}

    /** @alias xmlwriter_start_attribute */
    public function startAttribute(string $name): bool {}

    /** @alias xmlwriter_end_attribute */
    public function endAttribute(): bool {}

    /** @alias xmlwriter_write_attribute */
    public function writeAttribute(string $name, string $value): bool {}

    /** @alias xmlwriter_start_attribute_ns */
    public function startAttributeNs(?string $prefix, string $name, ?string $uri): bool {}

    /** @alias xmlwriter_write_attribute_ns */
    public function writeAttributeNs(?string $prefix, string $name, ?string $uri, string $content): bool {}

    /** @alias xmlwriter_start_element */
    public function startElement(string $name): bool {}

    /** @alias xmlwriter_end_element */
    public function endElement(): bool {}

    /** @alias xmlwriter_full_end_element */
    public function fullEndElement(): bool {}

    /** @alias xmlwriter_start_element_ns */
    public function startElementNs(?string $prefix, string $name, ?string $uri): bool {}

    /** @alias xmlwriter_write_element */
    public function writeElement(string $name, ?string $content = null): bool {}

    /** @alias xmlwriter_write_element_ns */
    public function writeElementNs(?string $prefix, string $name, ?string $uri, ?string $content = null): bool {}

    /** @alias xmlwriter_start_pi */
    public function startPi(string $target): bool {}

    /** @alias xmlwriter_end_pi */
    public function endPi(): bool {}

    /** @alias xmlwriter_write_pi */
    public function writePi(string $target, string $content): bool {}

    /** @alias xmlwriter_start_cdata */
    public function startCdata(): bool {}

    /** @alias xmlwriter_end_cdata */
    public function endCdata(): bool {}

    /** @alias xmlwriter_write_cdata */
    public function writeCdata(string $content): bool {}

    /** @alias xmlwriter_text */
    public function text(string $content): bool {}

    /** @alias xmlwriter_write_raw */
    public function writeRaw(string $content): bool {}

    /** @alias xmlwriter_start_document */
    public function startDocument(?string $version = '1.0', ?string $encoding = null, ?string $standalone = null): bool {}

    /** @alias xmlwriter_end_document */
    public function endDocument(): bool {}

    /** @alias xmlwriter_write_comment */
    public function writeComment(string $content): bool {}

    /** @alias xmlwriter_start_dtd */
    public function startDtd(string $qualifiedName, ?string $publicId = null, ?string $systemId = null): bool {}

    /** @alias xmlwriter_end_dtd */
    public function endDtd(): bool {}

    /** @alias xmlwriter_write_dtd */
    public function writeDtd(string $name, ?string $publicId = null, ?string $systemId = null, ?string $subset = null): bool {}

    /** @alias xmlwriter_start_dtd_element */
    public function startDtdElement(string $qualifiedName): bool {}

    /** @alias xmlwriter_end_dtd_element */
    public function endDtdElement(): bool {}

    /** @alias xmlwriter_write_dtd_element */
    public function writeDtdElement(string $name, string $content): bool {}

    /** @alias xmlwriter_start_dtd_attlist */
    public function startDtdAttlist(string $name): bool {}

    /** @alias xmlwriter_end_dtd_attlist */
    public function endDtdAttlist(): bool {}

    /** @alias xmlwriter_write_dtd_attlist */
    public function writeDtdAttlist(string $name, string $content): bool {}

    /** @alias xmlwriter_start_dtd_entity */
    public function startDtdEntity(string $name, bool $isparam): bool {}

    /** @alias xmlwriter_end_dtd_entity */
    public function endDtdEntity(): bool {}

    /** @alias xmlwriter_write_dtd_entity */
    public function writeDtdEntity(string $name, string $content, bool $isparam, string $publicId = UNKNOWN, string $systemId = UNKNOWN, string $ndataid = UNKNOWN): bool {}

    /** @alias xmlwriter_output_memory */
    public function outputMemory(bool $flush = true): string {}

    /** @alias xmlwriter_flush */
    public function flush(bool $empty = true): string|int {}
}
