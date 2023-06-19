<?php

/** @generate-class-entries */

namespace Url;

enum UrlComponent: int {
    case Scheme = 0;
    case Host = 1;
    case Port = 2;
    case User = 3;
    case Password = 4;
    case Path = 5;
    case Query = 6;
    case Fragment = 7;
}

readonly class Url implements \Stringable {
    public ?string $scheme;
    public ?string $host;
    public ?string $port;
    public ?string $user;
    public ?string $password;
    public ?string $path;
    public ?string $query;
    public ?string $fragment;

    public function __construct(
        string $scheme,
        string $host,
        string $port,
        string $user,
        string $password,
        string $path,
        string $query,
        string $fragment
    ) {}

    public function __toString(): string {}
}

final class UrlParser
{
    /**
     * @refcount 1
     */
    public static function parseUrl(string $url): Url {}

    /**
     * @return array<string, string>
     * @refcount 1
     */
    public static function parseUrlArray(string $url): array {}

    public static function parseUrlComponent(string $url, UrlComponent $component): string|null {}
}
