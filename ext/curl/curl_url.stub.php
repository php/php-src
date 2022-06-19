<?php

/** @generate-class-entries */

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
/**
 * @strict-properties
 * @not-serializable
 */
final class CurlUrl implements Stringable
{
    /**
     * @var int
     * @cname CURLU_APPENDQUERY
     */
    public const APPENDQUERY = UNKNOWN;

    /**
     * @var int
     * @cname CURLU_DEFAULT_PORT
     */
    public const DEFAULT_PORT = UNKNOWN;

    /**
     * @var int
     * @cname CURLU_DEFAULT_SCHEME
     */
    public const DEFAULT_SCHEME = UNKNOWN;

    /**
     * @var int
     * @cname CURLU_DISALLOW_USER
     */
    public const DISALLOW_USER = UNKNOWN;

    /**
     * @var int 
     * @cname CURLU_GUESS_SCHEME
     */
    public const GUESS_SCHEME = UNKNOWN;

    /**
     * @var int
     * @cname CURLU_NO_DEFAULT_PORT
     */
    public const NO_DEFAULT_PORT = UNKNOWN;

    /**
     * @var int
     * @cname CURLU_NON_SUPPORT_SCHEME
     */
    public const NON_SUPPORT_SCHEME = UNKNOWN;

    /**
     * @var int
     * @cname CURLU_PATH_AS_IS
     */
    public const PATH_AS_IS = UNKNOWN;

    /**
     * @var int
     * @cname CURLU_URLDECODE
     */
    public const URLDECODE = UNKNOWN;

    /**
     * @var int
     * @cname CURLU_URLENCODE
     */
    public const URLENCODE = UNKNOWN;

#if LIBCURL_VERSION_NUM >= 0x074300 /* Available since 7.67.0 */
    /**
     * @var int
     * @cname CURLU_NO_AUTHORITY
     */
    public const NO_AUTHORITY = UNKNOWN;
#endif
      
#if LIBCURL_VERSION_NUM >= 0x074e00 /* Available since 7.78.0 */
    /**
     * @var int
     * @cname CURLU_ALLOW_SPACE
     */
    public const ALLOW_SPACE = UNKNOWN;
#endif

    public function __construct(?string $url = null, int $flags = 0) {}

    public function get(int $flags = 0): string {}
    public function set(?string $url, int $flags = 0): CurlUrl {}

    public function getHost(): ?string {}
    public function setHost(?string $host): CurlUrl {}

    public function getScheme(): ?string {}
    public function setScheme(?string $scheme, int $flags = 0): CurlUrl {}

    public function getPort(int $flags = 0): ?int {}
    public function setPort(?int $port): CurlUrl {}

    public function getPath(int $flags = 0): string {}
    public function setPath(?string $scheme, int $flags = 0): CurlUrl {}

    public function getQuery(int $flags = 0): ?string {}
    public function setQuery(?string $query, int $flags = 0): CurlUrl {}

    public function getFragment(int $flags = 0): ?string {}
    public function setFragment(?string $fragment, int $flags = 0): CurlUrl {}

    public function getUser(int $flags = 0): ?string {}
    public function setUser(?string $user, int $flags = 0): CurlUrl {}

    public function getPassword(int $flags = 0): ?string {}
    public function setPassword(?string $password, int $flags = 0): CurlUrl {}

    public function getOptions(int $flags = 0): ?string {}
    public function setOptions(?string $options, int $flags = 0): CurlUrl {}

#if LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
    public function getZoneId(int $flags = 0): ?string {}
    public function setZoneId(?string $zoneid, int $flags = 0): CurlUrl {}
#endif
 
    public function __toString(): string {}
}

final class CurlUrlException extends Exception
{
    /**
     * @var int
     * @cname CURLUE_BAD_PARTPOINTER
     */
    public const BAD_PARTPOINTER = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_PORT_NUMBER
     */
    public const BAD_PORT_NUMBER = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_MALFORMED_INPUT
     */
    public const MALFORMED_INPUT = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_OUT_OF_MEMORY
     */
    public const OUT_OF_MEMORY = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_UNSUPPORTED_SCHEME
     */
    public const UNSUPPORTED_SCHEME = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_URLDECODE
     */
    public const URLDECODE = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_USER_NOT_ALLOWED
     */
    public const USER_NOT_ALLOWED = UNKNOWN;

#if LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */
    /**
     * @var int
     * @cname CURLUE_BAD_FILE_URL
     */
    public const BAD_FILE_URL = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_FRAGMENT
     */
    public const BAD_FRAGMENT = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_HOSTNAME
     */
    public const BAD_HOSTNAME = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_IPV6
     */
    public const BAD_IPV6 = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_LOGIN
     */
    public const BAD_LOGIN = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_PASSWORD
     */
    public const BAD_PASSWORD = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_PATH
     */
    public const BAD_PATH = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_QUERY
     */
    public const BAD_QUERY = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_SCHEME
     */
    public const BAD_SCHEME = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_SLASHES
     */
    public const BAD_SLASHES = UNKNOWN;

    /**
     * @var int
     * @cname CURLUE_BAD_USER
     */
    public const BAD_USER = UNKNOWN;
#endif
}
#endif
