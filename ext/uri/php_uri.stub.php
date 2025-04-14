<?php

/** @generate-class-entries */

namespace Uri {
    /** @strict-properties */
    class UriException extends \Exception
    {
    }

    /** @strict-properties */
    class InvalidUriException extends \Uri\UriException
    {
    }
}

namespace Uri\Rfc3986 {
    /** @strict-properties */
    final readonly class Uri
    {
        public static function parse(string $uri, ?string $baseUrl = null): ?static {}

        public function __construct(string $uri, ?string $baseUrl = null) {}

        public function getScheme(): ?string {}

        public function getRawScheme(): ?string {}

        public function withScheme(?string $scheme): static {}

        public function getUserInfo(): ?string {}

        public function getRawUserInfo(): ?string {}

        public function withUserInfo(?string $userInfo): static {}

        public function getUsername(): ?string {}

        public function getRawUsername(): ?string {}

        public function getPassword(): ?string {}

        public function getRawPassword(): ?string {}

        public function getHost(): ?string {}

        public function getRawHost(): ?string {}

        public function withHost(?string $host): static {}

        public function getPort(): ?int {}

        public function withPort(?int $port): static {}

        public function getPath(): string {}

        public function getRawPath(): string {}

        public function withPath(string $path): static {}

        public function getQuery(): ?string {}

        public function getRawQuery(): ?string {}

        public function withQuery(?string $query): static {}

        public function getFragment(): ?string {}

        public function getRawFragment(): ?string {}

        public function withFragment(?string $fragment): static {}

        public function equals(\Uri\Rfc3986\Uri $uri, bool $excludeFragment = true): bool {}

        public function toString(): string {}

        public function toRawString(): string {}

        public function resolve(string $uri): static {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

        public function __debugInfo(): array {}
    }
}

namespace Uri\WhatWg {
    /** @strict-properties */
    class InvalidUrlException extends \Uri\InvalidUriException
    {
        public readonly array $errors;
    }

    enum UrlValidationErrorType
    {
        case DomainToAscii;
        case DomainToUnicode;
        case DomainInvalidCodePoint;
        case HostInvalidCodePoint;
        case Ipv4EmptyPart;
        case Ipv4TooManyParts;
        case Ipv4NonNumericPart;
        case Ipv4NonDecimalPart;
        case Ipv4OutOfRangePart;
        case Ipv6Unclosed;
        case Ipv6InvalidCompression;
        case Ipv6TooManyPieces;
        case Ipv6MultipleCompression;
        case Ipv6InvalidCodePoint;
        case Ipv6TooFewPieces;
        case Ipv4InIpv6TooManyPieces;
        case Ipv4InIpv6InvalidCodePoint;
        case Ipv4InIpv6OutOfRangePart;
        case Ipv4InIpv6TooFewParts;
        case InvalidUrlUnit;
        case SpecialSchemeMissingFollowingSolidus;
        case MissingSchemeNonRelativeUrl;
        case InvalidReverseSoldius;
        case InvalidCredentials;
        case HostMissing;
        case PortOutOfRange;
        case PortInvalid;
        case FileInvalidWindowsDriveLetter;
        case FileInvalidWindowsDriveLetterHost;
    }

    /** @strict-properties */
    final readonly class UrlValidationError
    {
        public string $context;
        public \Uri\WhatWg\UrlValidationErrorType $type;
        public bool $failure;

        public function __construct(string $context, \Uri\WhatWg\UrlValidationErrorType $type, bool $failure) {}
    }

    /** @strict-properties */
    final readonly class Url
    {
        /** @param array $errors */
        public static function parse(string $uri, ?string $baseUrl = null, &$errors = null): ?static {}

        /** @param array $softErrors */
        public function __construct(string $uri, ?string $baseUrl = null, &$softErrors = null) {}

        public function getScheme(): string {}

        public function withScheme(string $scheme): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getUsername */
        public function getUsername(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawUsername */
        public function getRawUsername(): ?string {}

        public function withUsername(?string $user): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPassword */
        public function getPassword(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawPassword */
        public function getRawPassword(): ?string {}

        public function withPassword(?string $password): static {}

        public function getAsciiHost(): ?string {}

        public function getUnicodeHost(): ?string {}

        public function withHost(?string $host): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPort */
        public function getPort(): ?int {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPort */
        public function withPort(?int $port): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPath */
        public function getPath(): string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawPath */
        public function getRawPath(): string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPath */
        public function withPath(string $path): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getQuery */
        public function getQuery(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawQuery */
        public function getRawQuery(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withQuery */
        public function withQuery(?string $query): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getFragment */
        public function getFragment(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawFragment */
        public function getRawFragment(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withFragment */
        public function withFragment(?string $fragment): static {}

        public function equals(\Uri\WhatWg\Url $url, bool $excludeFragment = true): bool {}

        public function toAsciiString(): string {}

        public function toUnicodeString(): string {}

        /** @implementation-alias Uri\Rfc3986\Uri::resolve */
        public function resolve(string $uri): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::__serialize */
        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

        /** @implementation-alias Uri\Rfc3986\Uri::__debugInfo */
        public function __debugInfo(): array {}
    }
}
