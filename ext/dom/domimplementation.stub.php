<?php

class DOMImplementation
{
    public function getFeature(string $feature, string $version) {}

    /** @return bool */
    public function hasFeature(string $feature, string $version) {}

    /** @return DOMDocumentType|false */
    public function createDocumentType(
        string $qualifiedName, string $publicId = "", string $systemId = "") {}

    /** @return DOMDocument|false */
    public function createDocument(string $namespaceURI = "", string $qualifiedName = "", DOMDocumentType $doctype = UNKNOWN) {}
}
