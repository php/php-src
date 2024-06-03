<?php

/** @generate-class-entries */

namespace Uri;

/** @strict-properties */
final readonly class WhatWgError
{
    /** @cvalue LXB_URL_ERROR_TYPE_DOMAIN_TO_ASCII */
    public const int ERROR_TYPE_DOMAIN_TO_ASCII = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_DOMAIN_TO_UNICODE */
    public const int ERROR_TYPE_DOMAIN_TO_UNICODE = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT */
    public const int ERROR_TYPE_DOMAIN_INVALID_CODE_POINT = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_HOST_INVALID_CODE_POINT */
    public const int ERROR_TYPE_HOST_INVALID_CODE_POINT = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_EMPTY_PART */
    public const int ERROR_TYPE_IPV4_EMPTY_PART = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_TOO_MANY_PARTS */
    public const int ERROR_TYPE_IPV4_TOO_MANY_PARTS = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_NON_NUMERIC_PART */
    public const int ERROR_TYPE_IPV4_NON_NUMERIC_PART = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_NON_DECIMAL_PART */
    public const int ERROR_TYPE_IPV4_NON_DECIMAL_PART = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART */
    public const int ERROR_TYPE_IPV4_OUT_OF_RANGE_PART = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV6_UNCLOSED */
    public const int ERROR_TYPE_IPV6_UNCLOSED = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV6_INVALID_COMPRESSION */
    public const int ERROR_TYPE_IPV6_INVALID_COMPRESSION = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV6_TOO_MANY_PIECES */
    public const int ERROR_TYPE_IPV6_TOO_MANY_PIECES = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION */
    public const int ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV6_INVALID_CODE_POINT */
    public const int ERROR_TYPE_IPV6_INVALID_CODE_POINT = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV6_TOO_FEW_PIECES */
    public const int ERROR_TYPE_IPV6_TOO_FEW_PIECES = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES */
    public const int ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT */
    public const int ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART */
    public const int ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS */
    public const int ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_INVALID_URL_UNIT */
    public const int ERROR_TYPE_INVALID_URL_UNIT = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS */
    public const int ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL */
    public const int ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS */
    public const int ERROR_TYPE_INVALID_REVERSE_SOLIDUS = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_INVALID_CREDENTIALS */
    public const int ERROR_TYPE_INVALID_CREDENTIALS = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_HOST_MISSING */
    public const int ERROR_TYPE_HOST_MISSING = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_PORT_OUT_OF_RANGE */
    public const int ERROR_TYPE_PORT_OUT_OF_RANGE = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_PORT_INVALID */
    public const int ERROR_TYPE_PORT_INVALID = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER */
    public const int ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER = UNKNOWN;
    /** @cvalue LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST */
    public const int ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST = UNKNOWN;

    public string $position;
    public int $errorCode;

    public function __construct(string $position, int $errorCode) {}
}

interface UriInterface extends \Stringable
{
    public function getScheme(): ?string {}

    public function withScheme(?string $scheme): static {}

    public function getUser(): ?string {}

    public function withUser(?string $user): static {}

    public function getPassword(): ?string {}

    public function withPassword(?string $password): static {}

    public function getHost(): ?string {}

    public function withHost(?string $host): static {}

    public function getPort(): ?int {}

    public function withPort(?int $port): static {}

    public function getPath(): ?string {}

    public function withPath(?string $path): static {}

    public function getQuery(): ?string {}

    public function withQuery(?string $query): static {}

    public function getFragment(): ?string {}

    public function withFragment(?string $fragment): static {}

    public function __toString(): string {}
}

/** @strict-properties */
final readonly class Rfc3986Uri implements \Uri\UriInterface
{
    /** @virtual */
    private ?string $scheme;
    /** @virtual */
    private ?string $user;
    /** @virtual */
    private ?string $password;
    /** @virtual */
    private ?string $host;
    /** @virtual */
    private ?int $port;
    /** @virtual */
    private ?string $path;
    /** @virtual */
    private ?string $query;
    /** @virtual */
    private ?string $fragment;

    public static function create(string $uri, ?string $baseUrl = null): ?static {}

    public function __construct(string $uri, ?string $baseUrl = null) {}

    public function getScheme(): ?string {}

    public function withScheme(?string $scheme): static {}

    public function getUser(): ?string {}

    public function withUser(?string $user): static {}

    public function getPassword(): ?string {}

    public function withPassword(?string $password): static {}

    public function getHost(): ?string {}

    public function withHost(?string $host): static {}

    public function getPort(): ?int {}

    public function withPort(?int $port): static {}

    public function getPath(): ?string {}

    public function withPath(?string $path): static {}

    public function getQuery(): ?string {}

    public function withQuery(?string $query): static {}

    public function getFragment(): ?string {}

    public function withFragment(?string $fragment): static {}

    public function __toString(): string {}
}

/** @strict-properties */
final readonly class WhatWgUri implements \Uri\UriInterface
{
    /** @virtual */
    private ?string $scheme;
    /** @virtual */
    private ?string $user;
    /** @virtual */
    private ?string $password;
    /** @virtual */
    private ?string $host;
    /** @virtual */
    private ?int $port;
    /** @virtual */
    private ?string $path;
    /** @virtual */
    private ?string $query;
    /** @virtual */
    private ?string $fragment;

    /** @param array $errors */
    public static function create(string $uri, ?string $baseUrl = null, &$errors = null): ?static {}

    /** @param array $errors */
    public function __construct(string $uri, ?string $baseUrl = null, &$errors = null) {}

    /** @implementation-alias Uri\Rfc3986Uri::getScheme */
    public function getScheme(): ?string {}

    /** @implementation-alias Uri\Rfc3986Uri::withScheme */
    public function withScheme(?string $scheme): static {}

    /** @implementation-alias Uri\Rfc3986Uri::getUser */
    public function getUser(): ?string {}

    /** @implementation-alias Uri\Rfc3986Uri::withUser */
    public function withUser(?string $user): static {}

    /** @implementation-alias Uri\Rfc3986Uri::getPassword */
    public function getPassword(): ?string {}

    /** @implementation-alias Uri\Rfc3986Uri::withPassword */
    public function withPassword(?string $password): static {}

    /** @implementation-alias Uri\Rfc3986Uri::getHost */
    public function getHost(): ?string {}

    /** @implementation-alias Uri\Rfc3986Uri::withHost */
    public function withHost(?string $host): static {}

    /** @implementation-alias Uri\Rfc3986Uri::getPort */
    public function getPort(): ?int {}

    /** @implementation-alias Uri\Rfc3986Uri::withPort */
    public function withPort(?int $port): static {}

    /** @implementation-alias Uri\Rfc3986Uri::getPath */
    public function getPath(): ?string {}

    /** @implementation-alias Uri\Rfc3986Uri::withPath */
    public function withPath(?string $path): static {}

    /** @implementation-alias Uri\Rfc3986Uri::getQuery */
    public function getQuery(): ?string {}

    /** @implementation-alias Uri\Rfc3986Uri::withQuery */
    public function withQuery(?string $query): static {}

    /** @implementation-alias Uri\Rfc3986Uri::getFragment */
    public function getFragment(): ?string {}

    /** @implementation-alias Uri\Rfc3986Uri::withFragment */
    public function withFragment(?string $fragment): static {}

    /** @implementation-alias Uri\Rfc3986Uri::__toString */
    public function __toString(): string {}
}
