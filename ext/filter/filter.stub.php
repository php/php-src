<?php


/**
 * @generate-class-entries
 * @generate-c-enums
 */

namespace {
/**
 * @var int
 * @cvalue PARSE_POST
 */
const INPUT_POST = UNKNOWN;
/**
 * @var int
 * @cvalue PARSE_GET
 */
const INPUT_GET = UNKNOWN;
/**
 * @var int
 * @cvalue PARSE_COOKIE
 */
const INPUT_COOKIE = UNKNOWN;
/**
 * @var int
 * @cvalue PARSE_ENV
 */
const INPUT_ENV = UNKNOWN;
/**
 * @var int
 * @cvalue PARSE_SERVER
 */
const INPUT_SERVER = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_FLAG_NONE
 */
const FILTER_FLAG_NONE = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_REQUIRE_SCALAR
 */
const FILTER_REQUIRE_SCALAR = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_REQUIRE_ARRAY
 */
const FILTER_REQUIRE_ARRAY = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FORCE_ARRAY
 */
const FILTER_FORCE_ARRAY = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_NULL_ON_FAILURE
 */
const FILTER_NULL_ON_FAILURE = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_THROW_ON_FAILURE
 */
const FILTER_THROW_ON_FAILURE = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_VALIDATE_INT
 */
const FILTER_VALIDATE_INT = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_VALIDATE_BOOL
 */
const FILTER_VALIDATE_BOOLEAN = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_VALIDATE_BOOL
 */
const FILTER_VALIDATE_BOOL = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_VALIDATE_FLOAT
 */
const FILTER_VALIDATE_FLOAT = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_VALIDATE_REGEXP
 */
const FILTER_VALIDATE_REGEXP = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_VALIDATE_DOMAIN
 */
const FILTER_VALIDATE_DOMAIN = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_VALIDATE_URL
 */
const FILTER_VALIDATE_URL = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_VALIDATE_EMAIL
 */
const FILTER_VALIDATE_EMAIL = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_VALIDATE_IP
 */
const FILTER_VALIDATE_IP = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_VALIDATE_MAC
 */
const FILTER_VALIDATE_MAC = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_DEFAULT
 */
const FILTER_DEFAULT = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_UNSAFE_RAW
 */
const FILTER_UNSAFE_RAW = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_SANITIZE_STRING
 */
#[\Deprecated(since: '8.1', message: 'use htmlspecialchars() instead')]
const FILTER_SANITIZE_STRING = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_STRING
 */
#[\Deprecated(since: '8.1', message: 'use htmlspecialchars() instead')]
const FILTER_SANITIZE_STRIPPED = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_ENCODED
 */
const FILTER_SANITIZE_ENCODED = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_SPECIAL_CHARS
 */
const FILTER_SANITIZE_SPECIAL_CHARS = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_FULL_SPECIAL_CHARS
 */
const FILTER_SANITIZE_FULL_SPECIAL_CHARS = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_EMAIL
 */
const FILTER_SANITIZE_EMAIL = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_URL
 */
const FILTER_SANITIZE_URL = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_NUMBER_INT
 */
const FILTER_SANITIZE_NUMBER_INT = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_NUMBER_FLOAT
 */
const FILTER_SANITIZE_NUMBER_FLOAT = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_SANITIZE_ADD_SLASHES
 */
const FILTER_SANITIZE_ADD_SLASHES = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_CALLBACK
 */
const FILTER_CALLBACK = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_FLAG_ALLOW_OCTAL
 */
const FILTER_FLAG_ALLOW_OCTAL = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_ALLOW_HEX
 */
const FILTER_FLAG_ALLOW_HEX = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_FLAG_STRIP_LOW
 */
const FILTER_FLAG_STRIP_LOW = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_STRIP_HIGH
 */
const FILTER_FLAG_STRIP_HIGH = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_STRIP_BACKTICK
 */
const FILTER_FLAG_STRIP_BACKTICK = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_ENCODE_LOW
 */
const FILTER_FLAG_ENCODE_LOW = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_ENCODE_HIGH
 */
const FILTER_FLAG_ENCODE_HIGH = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_ENCODE_AMP
 */
const FILTER_FLAG_ENCODE_AMP = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_NO_ENCODE_QUOTES
 */
const FILTER_FLAG_NO_ENCODE_QUOTES = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_EMPTY_STRING_NULL
 */
const FILTER_FLAG_EMPTY_STRING_NULL = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_FLAG_ALLOW_FRACTION
 */
const FILTER_FLAG_ALLOW_FRACTION = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_ALLOW_THOUSAND
 */
const FILTER_FLAG_ALLOW_THOUSAND = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_ALLOW_SCIENTIFIC
 */
const FILTER_FLAG_ALLOW_SCIENTIFIC = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_FLAG_PATH_REQUIRED
 */
const FILTER_FLAG_PATH_REQUIRED = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_QUERY_REQUIRED
 */
const FILTER_FLAG_QUERY_REQUIRED = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_FLAG_IPV4
 */
const FILTER_FLAG_IPV4 = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_IPV6
 */
const FILTER_FLAG_IPV6 = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_NO_RES_RANGE
 */
const FILTER_FLAG_NO_RES_RANGE = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_NO_PRIV_RANGE
 */
const FILTER_FLAG_NO_PRIV_RANGE = UNKNOWN;
/**
 * @var int
 * @cvalue FILTER_FLAG_GLOBAL_RANGE
 */
const FILTER_FLAG_GLOBAL_RANGE = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_FLAG_HOSTNAME
 */
const FILTER_FLAG_HOSTNAME = UNKNOWN;

/**
 * @var int
 * @cvalue FILTER_FLAG_EMAIL_UNICODE
 */
const FILTER_FLAG_EMAIL_UNICODE = UNKNOWN;

function filter_has_var(int $input_type, string $var_name): bool {}

function filter_input(int $type, string $var_name, Filter\Filter|int $filter = Filter\Filter::DEFAULT, array|Filter\Flag|int $options = 0): mixed {}

function filter_var(mixed $value, Filter\Filter|int $filter = Filter\Filter::DEFAULT, array|Filter\Flag|int $options = 0): mixed {}

/** @refcount 1 */
function filter_input_array(int $type, array|Filter\Filter|int $options = Filter\Filter::DEFAULT, bool $add_empty = true): array|false|null {}

/** @refcount 1 */
function filter_var_array(array $array, array|Filter\Filter|int $options = Filter\Filter::DEFAULT, bool $add_empty = true): array|false {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function filter_list(): array {}

function filter_id(string $name): int|false {}

}

namespace Filter {

	class FilterException extends \Exception {}

	class FilterFailedException extends FilterException {}

	enum Filter: int
	{
		case DEFAULT = FILTER_DEFAULT;

		case VALIDATE_BOOL = FILTER_VALIDATE_BOOL;
		case VALIDATE_INT = FILTER_VALIDATE_INT;
		case VALIDATE_FLOAT = FILTER_VALIDATE_FLOAT;
		case VALIDATE_REGEXP = FILTER_VALIDATE_REGEXP;
		case VALIDATE_DOMAIN = FILTER_VALIDATE_DOMAIN;
		case VALIDATE_URL = FILTER_VALIDATE_URL;
		case VALIDATE_EMAIL = FILTER_VALIDATE_EMAIL;
		case VALIDATE_IP = FILTER_VALIDATE_IP;
		case VALIDATE_MAC = FILTER_VALIDATE_MAC;

		case SANITIZE_STRING = FILTER_SANITIZE_STRING;
		case SANITIZE_EMAIL = FILTER_SANITIZE_EMAIL;
		case SANITIZE_URL = FILTER_SANITIZE_URL;
		case SANITIZE_NUMBER_INT = FILTER_SANITIZE_NUMBER_INT;
		case SANITIZE_NUMBER_FLOAT = FILTER_SANITIZE_NUMBER_FLOAT;
		case SANITIZE_SPECIAL_CHARS = FILTER_SANITIZE_SPECIAL_CHARS;
		case SANITIZE_FULL_SPECIAL_CHARS = FILTER_SANITIZE_FULL_SPECIAL_CHARS;
		case SANITIZE_ADD_SLASHES = FILTER_SANITIZE_ADD_SLASHES;

		case CALLBACK = FILTER_CALLBACK;
	}


	enum Flag: int
	{
		case NONE = FILTER_FLAG_NONE;

		case ALLOW_OCTAL = FILTER_FLAG_ALLOW_OCTAL;
		case ALLOW_HEX = FILTER_FLAG_ALLOW_HEX;

		case STRIP_LOW = FILTER_FLAG_STRIP_LOW;
		case STRIP_HIGH = FILTER_FLAG_STRIP_HIGH;
		case STRIP_BACKTICK = FILTER_FLAG_STRIP_BACKTICK;

		case ENCODE_LOW = FILTER_FLAG_ENCODE_LOW;
		case ENCODE_HIGH = FILTER_FLAG_ENCODE_HIGH;
		case ENCODE_AMP = FILTER_FLAG_ENCODE_AMP;
		case NO_ENCODE_QUOTES = FILTER_FLAG_NO_ENCODE_QUOTES;
		case EMPTY_STRING_NULL = FILTER_FLAG_EMPTY_STRING_NULL;

		case ALLOW_FRACTION = FILTER_FLAG_ALLOW_FRACTION;
		case ALLOW_THOUSAND = FILTER_FLAG_ALLOW_THOUSAND;
		case ALLOW_SCIENTIFIC = FILTER_FLAG_ALLOW_SCIENTIFIC;

		case PATH_REQUIRED = FILTER_FLAG_PATH_REQUIRED;
		case QUERY_REQUIRED = FILTER_FLAG_QUERY_REQUIRED;

		case IPV4 = FILTER_FLAG_IPV4;
		case IPV6 = FILTER_FLAG_IPV6;
		case NO_RES_RANGE = FILTER_FLAG_NO_RES_RANGE;
		case NO_PRIV_RANGE = FILTER_FLAG_NO_PRIV_RANGE;
		case GLOBAL_RANGE = FILTER_FLAG_GLOBAL_RANGE;
	}

}
