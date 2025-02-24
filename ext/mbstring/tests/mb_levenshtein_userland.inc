<?php
/* from: https://github.com/arabcoders/watchstate/blob/18e048b3a692aa0f46d34bde03c8518854907e4f/src/API/Backend/Mismatched.php#L331 */
/**
 * Copyright (c) 2024 ArabCoders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * Implementation levenshtein distance algorithm.
 *
 * @param string $str1 The first string.
 * @param string $str2 The second string.
 *
 * @return int The Levenshtein distance between the two strings.
 */
function watchstate_mb_levenshtein(string $str1, string $str2): int
{
	$length1 = mb_strlen($str1, 'UTF-8');
	$length2 = mb_strlen($str2, 'UTF-8');

	if ($length1 < $length2) {
		return watchstate_mb_levenshtein($str2, $str1);
	}

	if (0 === $length1) {
		return $length2;
	}

	if ($str1 === $str2) {
		return 0;
	}

	$prevRow = range(0, $length2);

	for ($i = 0; $i < $length1; $i++) {
		$currentRow = [];
		$currentRow[0] = $i + 1;
		$c1 = mb_substr($str1, $i, 1, 'UTF-8');

		for ($j = 0; $j < $length2; $j++) {
			$c2 = mb_substr($str2, $j, 1, 'UTF-8');
			$insertions = $prevRow[$j + 1] + 1;
			$deletions = $currentRow[$j] + 1;
			$substitutions = $prevRow[$j] + (($c1 !== $c2) ? 1 : 0);
			$currentRow[] = min($insertions, $deletions, $substitutions);
		}

		$prevRow = $currentRow;
	}
	return $prevRow[$length2];
}

