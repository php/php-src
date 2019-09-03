<?php

/** @return resource|false */
function xmlwriter_open_uri(string $uri) {}

/** @return resource|false */
function xmlwriter_open_memory() {}

/** @param resource $xmlwriter */
function xmlwriter_set_indent($xmlwriter, bool $indent): bool {}

/** @param resource $xmlwriter */
function xmlwriter_set_indent_string($xmlwriter, string $indentString): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_comment($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_comment($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_attribute($xmlwriter, string $name): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_attribute($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_attribute($xmlwriter, string $name, string $value): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_attribute_ns($xmlwriter, string $prefix, string $name, string $uri): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_attribute_ns($xmlwriter, string $prefix, string $name, string $uri, string $content): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_element($xmlwriter, string $name): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_element($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_full_end_element($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_element_ns($xmlwriter, string $prefix, string $name, string $uri): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_element($xmlwriter, string $name, string $content = UNKNOWN): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_element_ns($xmlwriter, string $prefix, string $name, string $uri, string $content = UNKNOWN): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_pi($xmlwriter, string $target): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_pi($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_pi($xmlwriter, string $target, string $content): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_cdata($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_cdata($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_cdata($xmlwriter, string $content): bool {}

/** @param resource $xmlwriter */
function xmlwriter_text($xmlwriter, string $content): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_raw($xmlwriter, string $content): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_document($xmlwriter, string $version = '1.0', $encoding = UNKNOWN, $standalone = UNKNOWN) {}

/** @param resource $xmlwriter */
function xmlwriter_end_document($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_comment($xmlwriter, string $content): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_dtd($xmlwriter, string $qualifiedName, string $publicId = UNKNOWN, string $systemId = UNKNOWN): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_dtd($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_dtd($xmlwriter, string $name, $publicId = UNKNOWN, $systemId = UNKNOWN, $subset = UNKNOWN): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_dtd_element($xmlwriter, string $qualifiedName): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_dtd_element($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_dtd_element($xmlwriter, string $name, string $content): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_dtd_attlist($xmlwriter, string $name): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_dtd_attlist($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_dtd_attlist($xmlwriter, string $name, string $content): bool {}

/** @param resource $xmlwriter */
function xmlwriter_start_dtd_entity($xmlwriter, string $name, bool $isparam): bool {}

/** @param resource $xmlwriter */
function xmlwriter_end_dtd_entity($xmlwriter): bool {}

/** @param resource $xmlwriter */
function xmlwriter_write_dtd_entity($xmlwriter, string $name, string $content, bool $isparam, string $publicId = UNKNOWN, string $systemId = UNKNOWN, string $ndataid = UNKNOWN): bool {}

/** @param resource $xmlwriter */
function xmlwriter_output_memory($xmlwriter, bool $flush = true): string {}

/**
 * @param resource $xmlwriter
 * @return string|int
 */
function xmlwriter_flush($xmlwriter, bool $empty = true) {}

class XMLWriter
{
    function openUri(string $uri): bool {}

    function openMemory(): bool {}

    function setIndent(bool $indent): bool {}

    function setIdentString(string $indentString): bool {}

    function startComment(): bool {}

    function endComment(): bool {}

    function startAttribute(string $name): bool {}

    function endAttribute(): bool {}

    function writeAttribute(string $name, string $value): bool {}

    function startAttributeNs(string $prefix, string $name, string $uri): bool {}

    function writeAttributeNs(string $prefix, string $name, string $uri, string $content): bool {}

    function startElement(string $name): bool {}

    function endElement(): bool {}

    function fullEndElement(): bool {}

    function startElementNs(string $prefix, string $name, string $uri): bool {}

    function writeElement(string $name, string $content = UNKNOWN): bool {}

    function writeElementNs(string $prefix, string $name, string $uri, string $content = UNKNOWN): bool {}

    function startPi(string $target): bool {}

    function endPi(): bool {}

    function writePi(string $target, string $content): bool {}

    function startCdata(): bool {}

    function endCdata(): bool {}

    function writeCdata(string $content): bool {}

    function text(string $content): bool {}

    function writeRaw(string $content): bool {}

    function startDocument(string $version = '1.0', string $encoding = UNKNOWN, string $standalone = UNKNOWN): bool {}

    function endDocument(): bool {}

    function writeComment(string $content): bool {}

    function startDtd(string $qualifiedName, string $publicId = UNKNOWN, string $systemId = UNKNOWN): bool {}

    function endDtd(): bool {}

    function writeDtd(string $name, string $publicId = UNKNOWN, string $systemId = UNKNOWN, string $subset = UNKNOWN): bool {}

    function startDtdElement(string $qualifiedName): bool {}

    function endDtdElement(): bool {}

    function writeDtdElement(string $name, string $content): bool {}

    function startDtdAttlist(string $name): bool {}

    function endDtdAttlist(): bool {}

    function writeDtdAttlist(string $name, string $content): bool {}

    function startDtdEntity(string $name, bool $isparam): bool {}

    function endDtdEntity(): bool {}

    function writeDtdEntity(string $name, string $content, bool $isparam, string $publicId = UNKNOWN, string $systemId = UNKNOWN, string $ndataid = UNKNOWN): bool {}

    function outputMemory(bool $flush = true): string {}

    /** @return string|int */
    function flush(bool $empty = true) {}
}
