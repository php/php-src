<?php

/** @generate-class-entries */

namespace {
	/**
	 * @var int
	 * @cvalue PREG_PATTERN_ORDER
	 */
	const PREG_PATTERN_ORDER = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PREG_SET_ORDER
	 */
	const PREG_SET_ORDER = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PREG_OFFSET_CAPTURE
	 */
	const PREG_OFFSET_CAPTURE = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PREG_UNMATCHED_AS_NULL
	 */
	const PREG_UNMATCHED_AS_NULL = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PREG_SPLIT_NO_EMPTY
	 */
	const PREG_SPLIT_NO_EMPTY = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PREG_SPLIT_DELIM_CAPTURE
	 */
	const PREG_SPLIT_DELIM_CAPTURE = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PREG_SPLIT_OFFSET_CAPTURE
	 */
	const PREG_SPLIT_OFFSET_CAPTURE = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PREG_GREP_INVERT
	 */
	const PREG_GREP_INVERT = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PHP_PCRE_NO_ERROR
	 */
	const PREG_NO_ERROR = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PHP_PCRE_INTERNAL_ERROR
	 */
	const PREG_INTERNAL_ERROR = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PHP_PCRE_BACKTRACK_LIMIT_ERROR
	 */
	const PREG_BACKTRACK_LIMIT_ERROR = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PHP_PCRE_RECURSION_LIMIT_ERROR
	 */
	const PREG_RECURSION_LIMIT_ERROR = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PHP_PCRE_BAD_UTF8_ERROR
	 */
	const PREG_BAD_UTF8_ERROR = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PHP_PCRE_BAD_UTF8_OFFSET_ERROR
	 */
	const PREG_BAD_UTF8_OFFSET_ERROR = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PHP_PCRE_JIT_STACKLIMIT_ERROR
	 */
	const PREG_JIT_STACKLIMIT_ERROR = UNKNOWN;
	/**
	 * @var string
	 * @cvalue php_pcre_version
	 */
	const PCRE_VERSION = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PCRE2_MAJOR
	 */
	const PCRE_VERSION_MAJOR = UNKNOWN;
	/**
	 * @var int
	 * @cvalue PCRE2_MINOR
	 */
	const PCRE_VERSION_MINOR = UNKNOWN;
	/**
	 * @var bool
	 * @cvalue PHP_PCRE_JIT_SUPPORT
	 */
	const PCRE_JIT_SUPPORT = UNKNOWN;

	/**
	 * @param array $matches
	 * @frameless-function {"arity": 2}
	 */
	function preg_match(string $pattern, string $subject, &$matches = null, int $flags = 0, int $offset = 0): int|false {}

	/** @param array $matches */
	function preg_match_all(string $pattern, string $subject, &$matches = null, int $flags = 0, int $offset = 0): int|false {}

	/**
	 * @param int $count
	 * @return string|array<int|string, string>|null
	 * @frameless-function {"arity": 3}
	 */
	function preg_replace(string|array $pattern, string|array $replacement, string|array $subject, int $limit = -1, &$count = null): string|array|null {}

	/**
	 * @param int $count
	 * @return string|array<int|string, string>|null
	 */
	function preg_filter(string|array $pattern, string|array $replacement, string|array $subject, int $limit = -1, &$count = null): string|array|null {}

	/**
	 * @param int $count
	 * @return string|array<int|string, string>|null
	 */
	function preg_replace_callback(string|array $pattern, callable $callback, string|array $subject, int $limit = -1, &$count = null, int $flags = 0): string|array|null {}

	/** @param int $count */
	function preg_replace_callback_array(array $pattern, string|array $subject, int $limit = -1, &$count = null, int $flags = 0): string|array|null {}

	/**
	 * @return array<int|string, string|array>|false
	 * @refcount 1
	 */
	function preg_split(string $pattern, string $subject, int $limit = -1, int $flags = 0): array|false {}

	/** @compile-time-eval */
	function preg_quote(string $str, ?string $delimiter = null): string {}

	/** @refcount 1 */
	function preg_grep(string $pattern, array $array, int $flags = 0): array|false {}

	function preg_last_error(): int {}

	function preg_last_error_msg(): string {}
}

/* TODO: by default will be PCRE2_UTF;
  In  PCRE,  by  default, \d, \D, \s, \S, \w, and \W recognize only ASCII
  characters, even in UTF-8 mode. However, this can be changed by setting
  the PCRE2_UCP option.
  #ifdef PCRE2_UCP
  						coptions |= PCRE2_UCP;
  #endif
  // The \C escape sequence is unsafe in PCRE2_UTF mode
  coptions |= PCRE2_NEVER_BACKSLASH_C;
*/
namespace Regex {
	class CompilationError extends Exception {}

	/**
	 * @strict-properties
	 * @not-serializable
	 */
	final class CompiledRegex {
		/** @cvalue PCRE2_NO_AUTO_CAPTURE */
		public const int NO_AUTO_CAPTURE = UNKNOWN;
		/** @cvalue PCRE2_ANCHORED */
		public const int ANCHORED = UNKNOWN;
		/** @cvalue PCRE2_DUPNAMES */
		public const int DUPNAMES = UNKNOWN;
#ifdef PCRE2_EXTRA_CASELESS_RESTRICT
		/** @cvalue PCRE2_EXTRA_CASELESS_RESTRICT */
		public const int EXTRA_CASELESS_RESTRICT = UNKNOWN;
#endif

		public function __construct(
			string $pattern,
			bool $caseSensitive = true,
			bool $greedy = true,
			bool $anchor = false,
			bool $multiLine = false, // TODO Better name?
			bool $dotMatchesNewLine = false, // TODO Better name?
			bool $ignoreWhitespace = false,
			bool $captureOnlyNamedGroups = false,
			bool $allowDuplicateSubPatternNames = false,
		) {}
	}
}
