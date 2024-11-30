--TEST--
Test URI equality checks
--EXTENSIONS--
uri
--XFAIL--
Cloning Rfc3986Uris doesn't copy the path properly yet
--FILE--
<?php

readonly class MyRfc3986Uri extends Uri\Rfc3986Uri
{
}

readonly class MyWhatWgUri extends Uri\WhatWgUri
{
}

readonly class FakeUri implements Uri\Uri
{
    public function getScheme(): ?string
    {
        return "https";
    }

    public function withScheme(?string $scheme): static
    {
        return $this;
    }

    public function getUser(): ?string
    {
        return null;
    }

    public function withUser(?string $user): static
    {
        return $this;
    }

    public function getPassword(): ?string
    {
        return null;
    }

    public function withPassword(?string $password): static
    {
        return $this;
    }

    public function getHost(): ?string
    {
        return "example.com";
    }

    public function withHost(?string $host): static {
        return $this;
    }

    public function getPort(): ?int
    {
        return null;
    }

    public function withPort(?int $port): static
    {
        return $this;
    }

    public function getPath(): ?string
    {
        return null;
    }

    public function withPath(?string $path): static
    {
        return $this;
    }

    public function getQuery(): ?string
    {
        return null;
    }

    public function withQuery(?string $query): static
    {
        return $this;
    }

    public function getFragment(): ?string
    {
        return null;
    }

    public function withFragment(?string $fragment): static
    {
        return $this;
    }

    public function equals(Uri\Uri $uri, bool $excludeFragment = true): bool
    {
        return false;
    }

    public function normalize(): static
    {
        return $this;
    }

    public function toNormalizedString(): string
    {
        return "https://example.com";
    }

    public function toString(): string
    {
        return "https://example.com";
    }

    public function resolve(string $uri): static
    {
        return $this;
    }
}

var_dump(new Uri\Rfc3986Uri("https://example.com")->equals(new Uri\Rfc3986Uri("https://example.com")));
var_dump(new Uri\Rfc3986Uri("https://example.com#foo")->equals(new Uri\Rfc3986Uri("https://example.com#bar"), true));
var_dump(new Uri\Rfc3986Uri("https://example.com#foo")->equals(new Uri\Rfc3986Uri("https://example.com#bar"), false));
var_dump(new Uri\Rfc3986Uri("https://example.com/")->equals(new Uri\WhatWgUri("https://example.com/")));
var_dump(new Uri\Rfc3986Uri("https://example.com/foo/..")->equals(new Uri\Rfc3986Uri("https://example.com")));
var_dump(new Uri\Rfc3986Uri("http://example%2ecom/foo%2fb%61r")->equals(new Uri\Rfc3986Uri("http://example%2ecom/foo/bar")));

var_dump(new MyRfc3986Uri("https://example.com/foo/..")->equals(new Uri\Rfc3986Uri("https://example.com")));
var_dump(new Uri\Rfc3986Uri("https://example.com/foo/..")->equals(new MyRfc3986Uri("https://example.com")));
var_dump(new MyRfc3986Uri("https://example.com/")->equals(new MyWhatWgUri("https://example.com")));
var_dump(new FakeUri("https://example.com")->equals(new Uri\Rfc3986Uri("https://example.com")));
var_dump(new Uri\Rfc3986Uri("https://example.com")->equals(new FakeUri("https://example.com")));

var_dump(new Uri\WhatWgUri("https://example.com")->equals(new Uri\WhatWgUri("https://example.com")));
var_dump(new Uri\WhatWgUri("https://example.com#foo")->equals(new Uri\WhatWgUri("https://example.com#bar"), true));
var_dump(new Uri\WhatWgUri("https://example.com#foo")->equals(new Uri\WhatWgUri("https://example.com#bar"), false));
var_dump(new Uri\WhatWgUri("https://example.com/")->equals(new Uri\Rfc3986Uri("https://example.com/")));
var_dump(new Uri\WhatWgUri("https://example.com/foo/..")->equals(new Uri\WhatWgUri("https://example.com")));
var_dump(new Uri\WhatWgUri("http://example%2ecom/foo/bar")->equals(new Uri\WhatWgUri("http://example.com/foo/bar")));
var_dump(new Uri\WhatWgUri("http://example.com/foo%2fb%61r")->equals(new Uri\WhatWgUri("http://example%2ecom/foo/bar")));

var_dump(new MyWhatWgUri("https://example.com/foo/..")->equals(new Uri\WhatWgUri("https://example.com")));
var_dump(new Uri\WhatWgUri("https://example.com/foo/..")->equals(new MyWhatWgUri("https://example.com")));
var_dump(new MyWhatWgUri("https://example.com/")->equals(new MyRfc3986Uri("https://example.com")));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
