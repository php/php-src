<?php

/** @generate-class-entries */

class XSLTProcessor
{
    /**
     * @param DOMDocument|SimpleXMLElement $stylesheet
     * @tentative-return-type
     */
    public function importStylesheet(object $stylesheet): bool {}

    /**
     * @param DOMDocument|SimpleXMLElement $document
     * @tentative-return-type
     */
    public function transformToDoc(object $document, ?string $returnClass = null): object|false {}

    /**
     * @param DOMDocument|SimpleXMLElement $document
     * @tentative-return-type
     */
    public function transformToUri(object $document, string $uri): int {}

    /**
     * @param DOMDocument|SimpleXMLElement $document
     * @tentative-return-type
     */
    public function transformToXml(object $document): string|null|false {}

    /** @tentative-return-type */
    public function setParameter(string $namespace, array|string $name, ?string $value = null): bool {}

    /** @tentative-return-type */
    public function getParameter(string $namespace, string $name): string|false {}

    /** @tentative-return-type */
    public function removeParameter(string $namespace, string $name): bool {}

    /** @tentative-return-type */
    public function hasExsltSupport(): bool {}

    /** @tentative-return-type */
    public function registerPHPFunctions(array|string|null $functions = null): void {}

    /** @return bool */
    public function setProfiling(?string $filename) {} // TODO make the return type void

    /** @tentative-return-type */
    public function setSecurityPrefs(int $preferences): int {}

    /** @tentative-return-type */
    public function getSecurityPrefs(): int {}
}
