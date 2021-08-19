<?php

/** @generate-class-entries */

function use_soap_error_handler(bool $enable = true): bool {}

function is_soap_fault(mixed $object): bool {}

class SoapParam
{
    public string $param_name;
    public mixed $param_data;

    public function __construct(mixed $data, string $name) {}
}

class SoapHeader
{
    public string $namespace;
    public string $name;
    public mixed $data = null;
    public bool $mustUnderstand;
    public string|int|null $actor;

    public function __construct(string $namespace, string $name, mixed $data = UNKNOWN, bool $mustUnderstand = false, string|int|null $actor = null) {}
}

class SoapFault extends Exception
{
    public string $faultstring;
    public ?string $faultcode = null;
    public ?string $faultcodens = null;
    public ?string $faultactor = null;
    public mixed $detail = null;
    public ?string $_name = null;
    public mixed $headerfault = null;

    public function __construct(array|string|null $code, string $string, ?string $actor = null, mixed $details = null, ?string $name = null, mixed $headerFault = null) {}

    public function __toString(): string {}
}

class SoapVar
{
    public int $enc_type;
    public mixed $enc_value = null;
    public ?string $enc_stype = null;
    public ?string $enc_ns = null;
    public ?string $enc_name = null;
    public ?string $enc_namens = null;

    public function __construct(mixed $data, ?int $encoding, ?string $typeName = null, ?string $typeNamespace = null, ?string $nodeName = null, ?string $nodeNamespace = null) {}
}

class SoapServer
{
    /** @var resource */
    public $service;

    public ?SoapFault $__soap_fault = null;

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
    public ?string $uri = null;
    public ?int $style = null;
    public ?int $use = null;
    public ?string $location = null;
    public bool $trace = false;
    public ?int $compression = null;
    /** @var resource|null */
    public $sdl = null;
    /** @var resource|null */
    public $typemap = null;
    /** @var resource|null */
    public $httpsocket = null;
    /** @var resource|null */
    public $httpurl = null;

    public ?string $_login = null;
    public ?string $_password = null;
    public bool $_use_digest = false;
    public ?string $_digest = null;
    public ?string $_proxy_host = null;
    public ?int $_proxy_port = null;
    public ?string $_proxy_login = null;
    public ?string $_proxy_password = null;
    public bool $_exceptions = true;
    public ?string $_encoding = null;
    public ?array $_classmap = null;
    public ?int $_features = null;
    public int $_connection_timeout = 0;
    /** @var resource|null */
    public $_stream_context = null;
    public ?string $_user_agent = null;
    public bool $_keep_alive = true;
    public ?int $_ssl_method = null;
    public int $_soap_version;
    public ?int $_use_proxy = null;
    public ?array $_cookies = null;
    public ?array $__default_headers = null;
    public ?SoapFault $__soap_fault = null;
    public ?string $__last_request = null;
    public ?string $__last_response = null;
    public ?string $__last_request_headers = null;
    public ?string $__last_response_headers = null;

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
