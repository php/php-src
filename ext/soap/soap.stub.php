<?php

/** @generate-class-entries */

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

    /** @tentative-return-type */
    public function fault(string $code, string $string, string $actor = "", mixed $details = null, string $name = ""): void {}

    /** @tentative-return-type */
    public function addSoapHeader(SoapHeader $header): void {}

    /** @tentative-return-type */
    public function setPersistence(int $mode): void {}

    /** @tentative-return-type */
    public function setClass(string $class, mixed ...$args): void {}

    /** @tentative-return-type */
    public function setObject(object $object): void {}

    /** @tentative-return-type */
    public function getFunctions(): array {}

    /**
     * @param array|string|int $functions
     * @tentative-return-type
     */
    public function addFunction($functions): void {}

    /** @tentative-return-type */
    public function handle(?string $request = null): void {}
}

class SoapClient
{
    public function __construct(?string $wsdl, array $options = []) {}

    /** @tentative-return-type */
    public function __call(string $name, array $args): mixed {}

    /**
     * @param SoapHeader|array|null $inputHeaders
     * @param array $outputHeaders
     * @tentative-return-type
     */
    public function __soapCall(string $name, array $args, ?array $options = null, $inputHeaders = null, &$outputHeaders = null): mixed {}

    /** @tentative-return-type */
    public function __getFunctions(): ?array {}

    /** @tentative-return-type */
    public function __getTypes(): ?array {}

    /** @tentative-return-type */
    public function __getLastRequest(): ?string {}

    /** @tentative-return-type */
    public function __getLastResponse(): ?string {}

    /** @tentative-return-type */
    public function __getLastRequestHeaders(): ?string {}

    /** @tentative-return-type */
    public function __getLastResponseHeaders(): ?string {}

    /** @tentative-return-type */
    public function __doRequest(string $request, string $location, string $action, int $version, bool $oneWay = false): ?string {}

    /** @tentative-return-type */
    public function __setCookie(string $name, ?string $value = null): void {}

    /** @tentative-return-type */
    public function __getCookies(): array {}

    /**
     * @param SoapHeader|array|null $headers
     * @tentative-return-type
     */
    public function __setSoapHeaders($headers = null): bool {}

    /** @tentative-return-type */
    public function __setLocation(?string $location = null): ?string {}
}
