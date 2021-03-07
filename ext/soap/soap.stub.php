<?php

/** @generate-class-entries */

function use_soap_error_handler(bool $enable = true): bool {}

function is_soap_fault(mixed $object): bool {}

class SoapParam
{
    /** @var string */
    public $param_name;

    public mixed $param_data = null;

    public function __construct(mixed $data, string $name) {}
}

class SoapHeader
{
    /** @var string */
    public $namespace;

    /** @var string */
    public $name;

    public mixed $data = null;

    /** @var bool */
    public $mustUnderstand;

    /** @var string|int|null */
    public $actor;

    public function __construct(string $namespace, string $name, mixed $data = UNKNOWN, bool $mustUnderstand = false, string|int|null $actor = null) {}
}

class SoapFault extends Exception
{
    /** @var string */
    public $feaultstring;

    /** @var string|null */
    public $faultcode;

    /** @var string|null */
    public $faultactor;

    public mixed $detail = null;

    /** @var string|null */
    public $_name;

    public mixed $headerfault = null;

    public function __construct(array|string|null $code, string $string, ?string $actor = null, mixed $details = null, ?string $name = null, mixed $headerFault = null) {}

    public function __toString(): string {}
}

class SoapVar
{
    /** @var int */
    public $enc_type;

    public mixed $enc_value = null;

    /** @var string|null */
    public $enc_stype;

    /** @var string|null */
    public $enc_ns;

    /** @var string|null */
    public $enc_name;

    /** @var string|null */
    public $enc_namens;

    public function __construct(mixed $data, ?int $encoding, ?string $typeName = null, ?string $typeNamespace = null, ?string $nodeName = null, ?string $nodeNamespace = null) {}
}

class SoapServer
{
    /** @var array|null */
    public $__soap_fault;

    /** @var resource */
    public $service;

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
    /** @var string|null */
    public $uri;

    /** @var int|null */
    public $style;

    /** @var int|null */
    public $use;

    /** @var string|null */
    public $location;

    /** @var string|null */
    public $_login;

    /** @var string|null */
    public $_password;

    /** @var string|null */
    public $_digest;

    /** @var string|null */
    public $_proxy_host;

    /** @var int|null */
    public $_proxy_port;

    /** @var string|null */
    public $_proxy_login;

    /** @var string|null */
    public $_proxy_password;

    /** @var int|null */
    public $trace;

    /** @var bool|null */
    public $_exceptions;

    /** @var int|null */
    public $compression;

    /** @var string|null */
    public $encoding;

    /** @var array|null */
    public $_classmap;

    /** @var int|null */
    public $_features;

    /** @var int|null */
    public $_connection_timeout;

    /** @var resource|null */
    public $_stream_context;

    /** @var string|null */
    public $_user_agent;

    /** @var int|null */
    public $_keep_alive;

    /** @var int|null */
    public $_ssl_method;

    /** @var int */
    public $_soap_version;

    /** @var resource|null */
    public $sdl;

    /** @var resource|null */
    public $typemap;

    /** @var string|null */
    public $__last_request;

    /** @var string|null */
    public $__last_response;

    /** @var string|null */
    public $__last_request_headers;

    /** @var string|null */
    public $__last_response_headers;

    /** @var array|null */
    public $__default_headers;

    /** @var resource|null */
    public $httpsocket;

    /** @var int|null */
    public $_use_proxy;

    /** @var resource|null */
    public $httpurl;

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
