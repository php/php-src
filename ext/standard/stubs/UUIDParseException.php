<?php
/**
 * PHP stub with full documentation for the native UUIDParseException class.
 *
 * This class is available as polyfill for PHP 5 and 7 at
 * [GitHub](https://github.com/Fleshgrinder/php-uuid).
 */

/**
 * Thrown if parsing of a UUID from a string fails.
 *
 * ## Examples
 * ```
 * <?php
 *
 * try {
 *     UUID::parse('php');
 * }
 * catch (UUIDParseException $e) {
 *     echo $e->getMessage();  // Expected at least 32 characters, but got 3 characters
 *     echo $e->getInput();    // php
 *     echo $e->getPosition(); // 0
 * }
 *
 * try {
 *     UUID::parse('12345678-1234-1234-1234-123456789php');
 * }
 * catch (UUIDParseException $e) {
 *     echo $e->getMessage();  // Expected hexadecimal digit, but found 'p' (0x70)
 *     echo $e->getInput();    // 12345678-1234-1234-1234-123456789php
 *     echo $e->getPosition(); // 33
 * }
 *
 * try {
 *     UUID::parse('12345678-1234-1234-1234-123456789abcdef');
 * }
 * catch (UUIDParseException $e) {
 *     echo $e->getMessage();  // Expected no more than 32 hexadecimal digits
 *     echo $e->getInput();    // 12345678-1234-1234-1234-123456789abcdef
 *     echo $e->getPosition(); // 37
 * }
 *
 * ?>
 * ```
 *
 * @see UUID::parse()
 */
final class UUIDParseException extends Exception {
	/** @var string */
	private $input;

	/** @var int */
	private $position;

	/**
	 * Construct new UUID parse exception instance.
	 *
	 * @param string $reason why parsing the UUID string failed.
	 * @param string $input that should be parsed.
	 * @param int $position at which parsing failed.
	 * @param Throwable|null $previous error/exception that lead to this
	 *     failure, if any.
	 */
	public function __construct(string $reason, string $input, int $position = 0, ?Throwable $previous = null) { }

	/**
	 * Get the original input string that should have been parsed as a UUID.
	 *
	 * @return string
	 */
	public function getInput(): string { }

	/**
	 * Get the position in the input string where the parsing failure occurred.
	 *
	 * @return int
	 */
	public function getPosition(): int { }
}
