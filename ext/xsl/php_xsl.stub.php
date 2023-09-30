<?php

/** @generate-class-entries */

/** @var int */
const XSL_CLONE_AUTO = 0;
/** @var int */
const XSL_CLONE_NEVER = -1;
/** @var int */
const XSL_CLONE_ALWAYS = 1;

/**
 * @var int
 * @cvalue XSL_SECPREF_NONE
 */
const XSL_SECPREF_NONE = UNKNOWN;
/**
 * @var int
 * @cvalue XSL_SECPREF_READ_FILE
 */
const XSL_SECPREF_READ_FILE = UNKNOWN;
/**
 * @var int
 * @cvalue XSL_SECPREF_WRITE_FILE
 */
const XSL_SECPREF_WRITE_FILE = UNKNOWN;
/**
 * @var int
 * @cvalue XSL_SECPREF_CREATE_DIRECTORY
 */
const XSL_SECPREF_CREATE_DIRECTORY = UNKNOWN;
/**
 * @var int
 * @cvalue XSL_SECPREF_READ_NETWORK
 */
const XSL_SECPREF_READ_NETWORK = UNKNOWN;
/**
 * @var int
 * @cvalue XSL_SECPREF_WRITE_NETWORK
 */
const XSL_SECPREF_WRITE_NETWORK = UNKNOWN;
/**
 * @var int
 * @cvalue XSL_SECPREF_DEFAULT
 */
const XSL_SECPREF_DEFAULT = UNKNOWN;

/**
 * @var int
 * @cvalue LIBXSLT_VERSION
 */
const LIBXSLT_VERSION = UNKNOWN;
/**
 * @var string
 * @cvalue LIBXSLT_DOTTED_VERSION
 */
const LIBXSLT_DOTTED_VERSION = UNKNOWN;

#ifdef HAVE_XSL_EXSLT
/**
 * @var int
 * @cvalue LIBEXSLT_VERSION
 */
const LIBEXSLT_VERSION = UNKNOWN;
/**
 * @var string
 * @cvalue LIBEXSLT_DOTTED_VERSION
 */
const LIBEXSLT_DOTTED_VERSION = UNKNOWN;
#endif

class XSLTProcessor
{
    /** @var bool */
    public $doXInclude;

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

    /** @return true */
    public function setProfiling(?string $filename) {} // TODO make return type void

    /** @tentative-return-type */
    public function setSecurityPrefs(int $preferences): int {}

    /** @tentative-return-type */
    public function getSecurityPrefs(): int {}
}
