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
        public static function parse(string $uri, ?string $baseUrl = null): ?static {}

        public function __construct(string $uri, ?string $baseUrl = null) {}

        public function getScheme(): ?string {}

        public function getRawScheme(): ?string {}

        public function withScheme(?string $encodedScheme): static {}

        public function getUser(): ?string {}

        public function getRawUser(): ?string {}

        public function withUser(?string $encodedUser): static {}

        public function getPassword(): ?string {}

        public function getRawPassword(): ?string {}

        public function withPassword(?string $encodedPassword): static {}

        public function getHost(): ?string {}

        public function getRawHost(): ?string {}

        public function withHost(?string $encodedHost): static {}

        public function getPort(): ?int {}

        public function withPort(?int $port): static {}

        public function getPath(): ?string {}

        public function getRawPath(): ?string {}

        public function withPath(?string $encodedPath): static {}

        public function getQuery(): ?string {}

        public function getRawQuery(): ?string {}

        public function withQuery(?string $encodedQuery): static {}

        public function getFragment(): ?string {}

        public function getRawFragment(): ?string {}

        public function withFragment(?string $encodedFragment): static {}

        public function equals(\Uri\Rfc3986\Uri $uri, bool $excludeFragment = true): bool {}

        public function toNormalizedString(): string {}

        public function toString(): string {}

        public function resolve(string $uri): static {}

        public function __serialize(): array;

        public function __unserialize(array $data): void;

        public function __debugInfo(): array;
    }
}

namespace Uri\WhatWg {
    enum WhatWgErrorType
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
        case PortOfOfRange;
        case PortInvalid;
        case FileInvalidWindowsDriveLetter;
        case FileInvalidWindowsDriveLetterHost;
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
        /** @param array $errors */
        public static function parse(string $uri, ?string $baseUrl = null, &$errors = null): ?static {}

        /** @param array $softErrors */
        public function __construct(string $uri, ?string $baseUrl = null, &$softErrors = null) {}

        public function getScheme(): string {}

        public function getRawScheme(): string {}

        public function withScheme(string $encodedScheme): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getUser */
        public function getUser(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawUser */
        public function getRawUser(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withUser */
        public function withUser(?string $encodedUser): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPassword */
        public function getPassword(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawPassword */
        public function getRawPassword(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPassword */
        public function withPassword(?string $encodedPassword): static {}

        public function getHost(): string {}

        public function getHumanFriendlyHost(): string {}

        public function withHost(string $encodedHost): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPort */
        public function getPort(): ?int {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPort */
        public function withPort(?int $encodedPort): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getPath */
        public function getPath(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawPath */
        public function getRawPath(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withPath */
        public function withPath(?string $encodedPath): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getQuery */
        public function getQuery(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawQuery */
        public function getRawQuery(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withQuery */
        public function withQuery(?string $encodedQuery): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::getFragment */
        public function getFragment(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::getRawFragment */
        public function getRawFragment(): ?string {}

        /** @implementation-alias Uri\Rfc3986\Uri::withFragment */
        public function withFragment(?string $encodedFragment): static {}

        public function equals(\Uri\WhatWg\Url $uri, bool $excludeFragment = true): bool {}

        public function toMachineFriendlyString(): string {}

        public function toHumanFriendlyString(): string {}

        /** @implementation-alias Uri\Rfc3986\Uri::resolve */
        public function resolve(string $uri): static {}

        /** @implementation-alias Uri\Rfc3986\Uri::__serialize */
        public function __serialize(): array {}

        public function __unserialize(array $data): void {}

        public function __debugInfo(): array {}
    }
}
