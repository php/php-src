<?php

/**
 * @generate-class-entries
 * @generate-c-enums
 */

namespace Uri {
    /** @strict-properties */
    class UriException extends \Exception
    {
    }

    /** @strict-properties */
    class UriError extends \Error
    {
    }

    /** @strict-properties */
    class InvalidUriException extends \Uri\UriException
    {
    }

    enum UriComparisonMode
    {
        case IncludeFragment;
        case ExcludeFragment;
    }

    final readonly class QueryParamOptions
    {
        /** @virtual */
        public int $parsingMaxQueryStringLength;
        /** @virtual */
        public int $parsingMaxParamCount;
        /** @virtual */
        public string $trueValue;
        /** @virtual */
        public string $falseValue;
        /** @virtual */
        public bool $useNullAsEmptyString;

        public function __construct(
            int $parsingMaxQueryStringLength = 10000,
            int $parsingMaxParamCount = 1000,
            string $trueValue = "0",
            string $falseValue = "0",
            bool $useNullAsEmptyString = false
        ) {}

        public function __debugInfo(): array {}
    }

    final readonly class QueryParams implements \IteratorAggregate, \Countable
    {
        private QueryParamOptions $options;

        public static function parseRfc1866(string $queryString, ?QueryParamOptions $options = null): \Uri\QueryParams {}

        public static function parseRfc3986(string $queryString, ?QueryParamOptions $options = null): ?\Uri\QueryParams {}

        public static function parseWhatWg(string $queryString, ?QueryParamOptions $options = null): \Uri\QueryParams {}

        public static function fromArray(array $queryParams, ?QueryParamOptions $options = null): \Uri\QueryParams {}

        public function __construct(?QueryParamOptions $options = null) {}

        public function append(string $name, mixed $value): static {}

        public function delete(string $name): static {}

        public function deleteValue(string $name, ?string $value): static {}

        public function has(string $name): bool {}

        public function hasValue(string $name, mixed $value): bool {}

        public function getFirst(string $name): ?string {}

        public function getLast(string $name): ?string {}

        public function getAll(string $name): array {}

        public function list(): array {}

        public function getIterator(): \Traversable {}

        public function count(): int {}

        public function set(string $name, mixed $value): static {}

        public function sort(): static {}

        public function toArray(): array {}

        public function toRfc1866String(): string {}

        public function toRfc3986String(): string {}

        public function toWhatWgString(): string {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

        public function __debugInfo(): array {}
    }
}

namespace Uri\Rfc3986 {
    enum UriType
    {
        case AbsolutePathReference;
        case RelativePathReference;
        case NetworkPathReference;
        case Uri;
    }

    enum UriHostType
    {
        case IPv4;
        case IPv6;
        case IPvFuture;
        case RegisteredName;
    }

    final class UriBuilder
    {
        private ?string $scheme = null;
        private ?string $userinfo = null;
        private ?string $host = null;
        private ?int $port = null;
        private string $path = "";
        private ?string $query = null;
        private ?string $fragment = null;

        public function reset(): static {}

        public function setScheme(?string $scheme): static {}

        public function setUserInfo(#[\SensitiveParameter] ?string $userInfo): static {}

        public function setHost(?string $host): static {}

        public function setPort(?int $port): static {}

        public function setPath(string $path): static {}

        public function setQuery(?string $query): static {}

        public function setFragment(?string $fragment): static {}

        public function build(?\Uri\Rfc3986\Uri $baseUrl = null): \Uri\Rfc3986\Uri {}
    }

    /** @strict-properties */
    final readonly class Uri
    {
        public static function parse(string $uri, ?\Uri\Rfc3986\Uri $baseUrl = null): ?static {}

        public function __construct(string $uri, ?\Uri\Rfc3986\Uri $baseUrl = null) {}

        public function getUriType(): ?\Uri\Rfc3986\UriType {}

        public function getScheme(): ?string {}

        public function getRawScheme(): ?string {}

        public function withScheme(?string $scheme): static {}

        public function getUserInfo(): ?string {}

        public function getRawUserInfo(): ?string {}

        public function withUserInfo(#[\SensitiveParameter] ?string $userinfo): static {}

        public function getUsername(): ?string {}

        public function getRawUsername(): ?string {}

        public function getPassword(): ?string {}

        public function getRawPassword(): ?string {}

        public function getHost(): ?string {}

        public function getRawHost(): ?string {}

        public function getHostType(): ?\Uri\Rfc3986\UriHostType {}

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

        public function equals(\Uri\Rfc3986\Uri $uri, \Uri\UriComparisonMode $comparisonMode = \Uri\UriComparisonMode::ExcludeFragment): bool {}

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

        public function __construct(string $message = "", array $errors = [], int $code = 0, ?\Throwable $previous = null) {}
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

    enum UrlHostType
    {
        case IPv4;
        case IPv6;
        case Domain;
        case Opaque;
        case Empty;
    }

    /** @strict-properties */
    final readonly class Url
    {
        /** @param array $errors */
        public static function parse(string $uri, ?\Uri\WhatWg\Url $baseUrl = null, &$errors = null): ?static {}

        /** @param array $softErrors */
        public function __construct(string $uri, ?\Uri\WhatWg\Url $baseUrl = null, &$softErrors = null) {}

        public function getScheme(): string {}

        public function withScheme(string $scheme): static {}

        public function isSpecialScheme(): bool {}

        /** @implementation-alias Uri\Rfc3986\Uri::getUsername */
        public function getUsername(): ?string {}

        public function withUsername(?string $username): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPassword */
        public function getPassword(): ?string {}

        public function withPassword(#[\SensitiveParameter] ?string $password): static {}

        public function getAsciiHost(): ?string {}

        public function getUnicodeHost(): ?string {}

        public function getHostType(): ?\Uri\WhatWg\UrlHostType {}

        /** @implementation-alias Uri\Rfc3986\Uri::withHost */
        public function withHost(?string $host): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPort */
        public function getPort(): ?int {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPort */
        public function withPort(?int $port): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPath */
        public function getPath(): string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPath */
        public function withPath(string $path): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getQuery */
        public function getQuery(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withQuery */
        public function withQuery(?string $query): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getFragment */
        public function getFragment(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withFragment */
        public function withFragment(?string $fragment): static {}

        public function equals(\Uri\WhatWg\Url $url, \Uri\UriComparisonMode $comparisonMode = \Uri\UriComparisonMode::ExcludeFragment): bool {}

        public function toAsciiString(): string {}

        public function toUnicodeString(): string {}

        /** @param array $softErrors */
        public function resolve(string $uri, &$softErrors = null): static {}

        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

        public function __debugInfo(): array {}
    }
}
