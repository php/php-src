<?php

/** @generate-function-entries */

function use_soap_error_handler(bool $enable = true): bool {}

function is_soap_fault(mixed $object): bool {}

class SoapParam
{
    public function __construct(mixed $data, string $name) {}
}

class SoapHeader
{
    public function __construct(string $namespace, string $name, mixed $data = UNKNOWN, bool $mustUnderstand = false, string|int|null $actor = null) {}
}

class SoapFault extends Exception
{
    public function __construct(array|string|null $code, string $string, ?string $actor = null, mixed $details = null, ?string $name = null, mixed $headerFault = null) {}

    public function __toString(): string {}
}

class SoapVar
{
    public function __construct(mixed $data, ?int $encoding, ?string $typeName = null, ?string $typeNamespace = null, ?string $nodeName = null, ?string $nodeNamespace = null) {}
}

class SoapServer
{
    public function __construct(?string $wsdl, array $options = []) {}

    /** @return void */
    public function fault(string $code, string $string, string $actor = "", mixed $details = null, string $name = "") {}

    /** @return void */
    public function addSoapHeader(SoapHeader $header) {}

    /** @return void */
    public function setPersistence(int $mode) {}

    /** @return void */
    public function setClass(string $class, mixed ...$args) {}

    /** @return void */
    public function setObject(object $object) {}

    /** @return array */
    public function getFunctions() {}

    /**
     * @param array|string|int $functions
     * @return void
     */
    public function addFunction($functions) {}

    /** @return void */
    public function handle(?string $request = null) {}
}

class SoapClient
{
    public function __construct(?string $wsdl, array $options = []) {}

    /** @return mixed */
    public function __call(string $name, array $args) {}

    /**
     * @param SoapHeader|array|null $inputHeaders
     * @param array $outputHeaders
     * @return mixed
     */
    public function __soapCall(string $name, array $args, ?array $options = null, $inputHeaders = null, &$outputHeaders = null) {}

    /** @return array|null */
    public function __getFunctions() {}

    /** @return array|null */
    public function __getTypes() {}

    /** @return string|null */
    public function __getLastRequest() {}

    /** @return string|null */
    public function __getLastResponse() {}

    /** @return string|null */
    public function __getLastRequestHeaders() {}

    /** @return string|null */
    public function __getLastResponseHeaders() {}

    /** @return string|null */
    public function __doRequest(string $request, string $location, string $action, int $version, bool $oneWay = false) {}

    /** @return void */
    public function __setCookie(string $name, ?string $value = null) {}

    /** @return array */
    public function __getCookies() {}

    /**
     * @param SoapHeader|array|null $headers
     * @return bool
     */
    public function __setSoapHeaders($headers = null) {}

    /** @return string|null */
    public function __setLocation(?string $location = null) {}
}
