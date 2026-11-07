--TEST--
json_decode()/json_encode() handle special bytes at every position of a string, across vectorized chunk boundaries
--FILE--
<?php

$plain = str_repeat('abcdefghijklmnop', 3); /* 48 bytes, several 16-byte chunks */
$failures = 0;

/* Decoding: raw bytes placed inside a JSON string literal. */
$raw = [
	"\x00" => JSON_ERROR_CTRL_CHAR,
	"\x1f" => JSON_ERROR_CTRL_CHAR,
	"\xff" => JSON_ERROR_UTF8,
	"\xc3" => JSON_ERROR_UTF8,
	"\xed\xa0\x80" => JSON_ERROR_UTF8,
	"\x20" => JSON_ERROR_NONE,
	"\x7f" => JSON_ERROR_NONE,
	"é" => JSON_ERROR_NONE,
	"\u{2028}" => JSON_ERROR_NONE,
	"😀" => JSON_ERROR_NONE,
];
for ($len = 0; $len <= strlen($plain); $len++) {
	for ($pos = 0; $pos <= $len; $pos++) {
		$prefix = substr($plain, 0, $pos);
		$suffix = substr($plain, $pos, $len - $pos);
		foreach ($raw as $byte => $error) {
			$value = $prefix . $byte . $suffix;
			$decoded = json_decode('"' . $value . '"');
			if (json_last_error() !== $error || ($error === JSON_ERROR_NONE && $decoded !== $value)) {
				echo "decode failed: len=$len pos=$pos byte=", bin2hex($byte), "\n";
				$failures++;
			}
			if (json_validate('"' . $value . '"') !== ($error === JSON_ERROR_NONE)) {
				echo "validate failed: len=$len pos=$pos byte=", bin2hex($byte), "\n";
				$failures++;
			}
		}
		/* Escape sequences and a lone quote that terminates the string early. */
		foreach (['\\"' => '"', '\\\\' => '\\', '\\n' => "\n", '\\u00e9' => 'é', '\\ud83d\\ude00' => '😀'] as $esc => $char) {
			if (json_decode('"' . $prefix . $esc . $suffix . '"') !== $prefix . $char . $suffix) {
				echo "escape failed: len=$len pos=$pos esc=$esc\n";
				$failures++;
			}
		}
		json_decode('"' . $prefix . '"' . $suffix . '"');
		if (json_last_error() !== JSON_ERROR_SYNTAX) {
			echo "early quote failed: len=$len pos=$pos\n";
			$failures++;
		}
	}
}

/* Encoding: each byte that may need escaping, with and without JSON_UNESCAPED_UNICODE. */
$special = ['"', '\\', '/', '<', '>', '&', "'", "\n", "\x00", "\x1f", "\x7f", "é", "€", "😀", "\u{2028}", "\u{2029}"];
$flagsets = [0, JSON_UNESCAPED_UNICODE, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_LINE_TERMINATORS,
	JSON_UNESCAPED_SLASHES | JSON_HEX_TAG | JSON_HEX_AMP | JSON_HEX_APOS | JSON_HEX_QUOT];
for ($len = 0; $len <= strlen($plain); $len++) {
	for ($pos = 0; $pos <= $len; $pos++) {
		foreach ($special as $char) {
			$value = substr($plain, 0, $pos) . $char . substr($plain, $pos, $len - $pos);
			foreach ($flagsets as $flags) {
				$encoded = json_encode($value, $flags);
				if (json_decode($encoded) !== $value) {
					echo "encode roundtrip failed: len=$len pos=$pos char=", bin2hex($char), " flags=$flags\n";
					$failures++;
				}
				$escapes_unicode = !($flags & JSON_UNESCAPED_UNICODE);
				$escapes_lt = $escapes_unicode || !($flags & JSON_UNESCAPED_LINE_TERMINATORS);
				$verbatim = strlen($char) > 1 && ($char === "\u{2028}" || $char === "\u{2029}" ? !$escapes_lt : !$escapes_unicode);
				if (strlen($char) > 1 && $verbatim !== str_contains($encoded, $char)) {
					echo "encode escaping failed: len=$len pos=$pos char=", bin2hex($char), " flags=$flags\n";
					$failures++;
				}
			}
		}
		/* Invalid UTF-8 at every position. */
		$value = substr($plain, 0, $pos) . "\xff" . substr($plain, $pos, $len - $pos);
		if (json_encode($value, JSON_UNESCAPED_UNICODE) !== false || json_last_error() !== JSON_ERROR_UTF8
				|| json_encode($value, JSON_UNESCAPED_UNICODE | JSON_INVALID_UTF8_IGNORE) !== json_encode(str_replace("\xff", '', $value))) {
			echo "encode invalid UTF-8 failed: len=$len pos=$pos\n";
			$failures++;
		}
	}
}

echo $failures === 0 ? "OK\n" : "$failures failures\n";

?>
--EXPECT--
OK
