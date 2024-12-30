<?php

/** @generate-class-entries */

namespace Uri {
    /** @strict-properties */
    class UriException extends \Exception
    {
    }

    /** @strict-properties */
    class UninitializedUriException extends \Uri\UriException
    {
    }

    /** @strict-properties */
    class UriOperationException extends \Uri\UriException
    {
    }

    /** @strict-properties */
    class InvalidUriException extends \Uri\UriException
    {
        public readonly array $errors;
    }
}

namespace Uri\Rfc3986 {
    /** @strict-properties */
    readonly class Uri
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

        public static function parse(string $uri, ?string $baseUrl = null): ?static {}

        public function __construct(string $uri, ?string $baseUrl = null) {}

        public function getScheme(): ?string {}

        public function withScheme(?string $encodedScheme): static {}

        public function getUser(): ?string {}

        public function withUser(?string $encodedUser): static {}

        public function getPassword(): ?string {}

        public function withPassword(?string $encodedPassword): static {}

        public function getHost(): ?string {}

        public function withHost(?string $encodedHost): static {}

        public function getPort(): ?int {}

        public function withPort(?int $port): static {}

        public function getPath(): ?string {}

        public function withPath(?string $encodedPath): static {}

        public function getQuery(): ?string {}

        public function withQuery(?string $encodedQuery): static {}

        public function getFragment(): ?string {}

        public function withFragment(?string $encodedFragment): static {}

        public function equals(\Uri\Rfc3986\Uri $uri, bool $excludeFragment = true): bool {}

        public function normalize(): static {}

        public function toNormalizedString(): string {}

        public function toString(): string {}

        public function resolve(string $uri): static {}

        public function __serialize(): array;

        public function __unserialize(array $data): void;

        public function __debugInfo(): array;
    }
}

namespace Uri\WhatWg {
    enum WhatWgErrorType: int {
        case DomainToAscii = 0;
        case DomainToUnicode = 1;
        case DomainInvalidCodePoint = 2;
        case HostInvalidCodePoint = 3;
        case Ipv4EmptyPart = 4;
        case Ipv4TooManyParts = 5;
        case Ipv4NonNumericPart = 6;
        case Ipv4NonDecimalPart = 7;
        case Ipv4OutOfRangePart = 8;
        case Ipv6Unclosed = 9;
        case Ipv6InvalidCompression = 10;
        case Ipv6TooManyPieces = 11;
        case Ipv6MultipleCompression = 12;
        case Ipv6InvalidCodePoint = 13;
        case Ipv6TooFewPieces = 14;
        case Ipv4InIpv6TooManyPieces = 15;
        case Ipv4InIpv6InvalidCodePoint = 16;
        case Ipv4InIpv6OutOfRangePart = 17;
        case Ipv4InIpv6TooFewParts = 18;
        case InvalidUrlUnit = 19;
        case SpecialSchemeMissingFollowingSolidus = 20;
        case MissingSchemeNonRelativeUrl = 21;
        case InvalidReverseSoldius = 22;
        case InvalidCredentials = 23;
        case HostMissing = 24;
        case PortOfOfRange = 25;
        case PortInvalid = 26;
        case FileInvalidWindowsDriveLetter = 27;
        case FileInvalidWindowsDriveLetterHost = 28;
    }

    /** @strict-properties */
    readonly class WhatWgError
    {
        public string $context;
        public \Uri\WhatWg\WhatWgErrorType $type;

        public function __construct(string $context, \Uri\WhatWg\WhatWgErrorType $type) {}
    }

    /** @strict-properties */
    readonly class Url
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
        public static function parse(string $uri, ?string $baseUrl = null, &$errors = null): ?static {}

        /** @param array $softErrors */
        public function __construct(string $uri, ?string $baseUrl = null, &$softErrors = null) {}

        /** @implementation-alias Uri\Rfc3986\Uri::getScheme */
        public function getScheme(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withScheme */
        public function withScheme(?string $encodedScheme): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getUser */
        public function getUser(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withUser */
        public function withUser(?string $encodedUser): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPassword */
        public function getPassword(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPassword */
        public function withPassword(?string $encodedPassword): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getHost */
        public function getHost(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withHost */
        public function withHost(?string $encodedHost): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPort */
        public function getPort(): ?int {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPort */
        public function withPort(?int $encodedPort): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPath */
        public function getPath(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPath */
        public function withPath(?string $encodedPath): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getQuery */
        public function getQuery(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withQuery */
        public function withQuery(?string $encodedQuery): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getFragment */
        public function getFragment(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withFragment */
        public function withFragment(?string $encodedFragment): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::equals */
        public function equals(\Uri\WhatWg\Url $uri, bool $excludeFragment = true): bool {}

        /** @implementation-alias Uri\Rfc3986\Uri::toString */
        public function toString(): string {}

        /** @implementation-alias Uri\Rfc3986\Uri::resolve */
        public function resolve(string $uri): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::__serialize */
        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

        public function __debugInfo(): array {}
    }
}
