<?php

#if defined(LIBXML_XPATH_ENABLED)
class DOMXPath
{
    public function __construct(DOMDocument $doc, bool $registerNodeNS = true) {}

    /** @return mixed */
    public function evaluate(string $expr, ?DOMNode $context = null, bool $registerNodeNS = true) {}

    /** @return mixed */
    public function query(string $expr, ?DOMNode $context = null, bool $registerNodeNS = true) {}

    /** @return bool */
    public function registerNamespace(string $prefix, string $namespaceURI) {}

    /**
     * @param string|array $restrict
     * @return bool|null
     */
    public function registerPhpFunctions($restrict = null) {}
}
#endif
