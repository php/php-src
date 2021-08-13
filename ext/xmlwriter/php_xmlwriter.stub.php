<?php

/** @generate-class-entries */

function xmlwriter_open_uri(string $uri): XMLWriter|false {}

function xmlwriter_open_memory(): XMLWriter|false {}

function xmlwriter_set_indent(XMLWriter $writer, bool $enable): bool {}

function xmlwriter_set_indent_string(XMLWriter $writer, string $indentation): bool {}

function xmlwriter_start_comment(XMLWriter $writer): bool {}

function xmlwriter_end_comment(XMLWriter $writer): bool {}

function xmlwriter_start_attribute(XMLWriter $writer, string $name): bool {}

function xmlwriter_end_attribute(XMLWriter $writer): bool {}

function xmlwriter_write_attribute(XMLWriter $writer, string $name, string $value): bool {}

function xmlwriter_start_attribute_ns(XMLWriter $writer, ?string $prefix, string $name, ?string $namespace): bool {}

function xmlwriter_write_attribute_ns(XMLWriter $writer, ?string $prefix, string $name, ?string $namespace, string $value): bool {}

function xmlwriter_start_element(XMLWriter $writer, string $name): bool {}

function xmlwriter_end_element(XMLWriter $writer): bool {}

function xmlwriter_full_end_element(XMLWriter $writer): bool {}

function xmlwriter_start_element_ns(XMLWriter $writer, ?string $prefix, string $name, ?string $namespace): bool {}

function xmlwriter_write_element(XMLWriter $writer, string $name, ?string $content = null): bool {}

function xmlwriter_write_element_ns(XMLWriter $writer, ?string $prefix, string $name, ?string $namespace, ?string $content = null): bool {}

function xmlwriter_start_pi(XMLWriter $writer, string $target): bool {}

function xmlwriter_end_pi(XMLWriter $writer): bool {}

function xmlwriter_write_pi(XMLWriter $writer, string $target, string $content): bool {}

function xmlwriter_start_cdata(XMLWriter $writer): bool {}

function xmlwriter_end_cdata(XMLWriter $writer): bool {}

function xmlwriter_write_cdata(XMLWriter $writer, string $content): bool {}

function xmlwriter_text(XMLWriter $writer, string $content): bool {}

function xmlwriter_write_raw(XMLWriter $writer, string $content): bool {}

function xmlwriter_start_document(XMLWriter $writer, ?string $version = "1.0", ?string $encoding = null, ?string $standalone = null): bool {}

function xmlwriter_end_document(XMLWriter $writer): bool {}

function xmlwriter_write_comment(XMLWriter $writer, string $content): bool {}

function xmlwriter_start_dtd(XMLWriter $writer, string $qualifiedName, ?string $publicId = null, ?string $systemId = null): bool {}

function xmlwriter_end_dtd(XMLWriter $writer): bool {}

function xmlwriter_write_dtd(XMLWriter $writer, string $name, ?string $publicId = null, ?string $systemId = null, ?string $content = null): bool {}

function xmlwriter_start_dtd_element(XMLWriter $writer, string $qualifiedName): bool {}

function xmlwriter_end_dtd_element(XMLWriter $writer): bool {}

function xmlwriter_write_dtd_element(XMLWriter $writer, string $name, string $content): bool {}

function xmlwriter_start_dtd_attlist(XMLWriter $writer, string $name): bool {}

function xmlwriter_end_dtd_attlist(XMLWriter $writer): bool {}

function xmlwriter_write_dtd_attlist(XMLWriter $writer, string $name, string $content): bool {}

function xmlwriter_start_dtd_entity(XMLWriter $writer, string $name, bool $isParam): bool {}

function xmlwriter_end_dtd_entity(XMLWriter $writer): bool {}

function xmlwriter_write_dtd_entity(XMLWriter $writer, string $name, string $content, bool $isParam = false, ?string $publicId = null, ?string $systemId = null, ?string $notationData = null): bool {}

function xmlwriter_output_memory(XMLWriter $writer, bool $flush = true): string {}

function xmlwriter_flush(XMLWriter $writer, bool $empty = true): string|int {}

class XMLWriter
{
    /**
     * @tentative-return-type
     * @alias xmlwriter_open_uri
     * @no-verify Behaviour differs from the aliased function
     */
    public function openUri(string $uri): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_open_memory
     * @no-verify Behaviour differs from the aliased function
     */
    public function openMemory(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_set_indent
     */
    public function setIndent(bool $enable): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_set_indent_string
     */
    public function setIndentString(string $indentation): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_comment
     */
    public function startComment(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_comment
     */
    public function endComment(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_attribute
     */
    public function startAttribute(string $name): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_attribute
     */
    public function endAttribute(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_attribute
     */
    public function writeAttribute(string $name, string $value): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_attribute_ns
     */
    public function startAttributeNs(?string $prefix, string $name, ?string $namespace): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_attribute_ns
     */
    public function writeAttributeNs(?string $prefix, string $name, ?string $namespace, string $value): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_element
     */
    public function startElement(string $name): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_element
     */
    public function endElement(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_full_end_element
     */
    public function fullEndElement(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_element_ns
     */
    public function startElementNs(?string $prefix, string $name, ?string $namespace): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_element
     */
    public function writeElement(string $name, ?string $content = null): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_element_ns
     */
    public function writeElementNs(?string $prefix, string $name, ?string $namespace, ?string $content = null): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_pi
     */
    public function startPi(string $target): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_pi
     */
    public function endPi(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_pi
     */
    public function writePi(string $target, string $content): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_cdata
     */
    public function startCdata(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_cdata
     * */
    public function endCdata(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_cdata
     */
    public function writeCdata(string $content): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_text
     */
    public function text(string $content): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_raw
     */
    public function writeRaw(string $content): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_document
     */
    public function startDocument(?string $version = "1.0", ?string $encoding = null, ?string $standalone = null): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_document
     */
    public function endDocument(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_comment
     */
    public function writeComment(string $content): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_dtd
     */
    public function startDtd(string $qualifiedName, ?string $publicId = null, ?string $systemId = null): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_dtd
     */
    public function endDtd(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_dtd
     */
    public function writeDtd(string $name, ?string $publicId = null, ?string $systemId = null, ?string $content = null): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_dtd_element
     */
    public function startDtdElement(string $qualifiedName): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_dtd_element
     */
    public function endDtdElement(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_dtd_element
     */
    public function writeDtdElement(string $name, string $content): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_dtd_attlist
     */
    public function startDtdAttlist(string $name): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_dtd_attlist
     */
    public function endDtdAttlist(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_dtd_attlist
     */
    public function writeDtdAttlist(string $name, string $content): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_start_dtd_entity
     */
    public function startDtdEntity(string $name, bool $isParam): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_end_dtd_entity
     */
    public function endDtdEntity(): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_write_dtd_entity
     */
    public function writeDtdEntity(string $name, string $content, bool $isParam = false, ?string $publicId = null, ?string $systemId = null, ?string $notationData = null): bool {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_output_memory
     */
    public function outputMemory(bool $flush = true): string {}

    /**
     * @tentative-return-type
     * @alias xmlwriter_flush
     */
    public function flush(bool $empty = true): string|int {}
}
