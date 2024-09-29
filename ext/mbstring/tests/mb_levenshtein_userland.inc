<?php
/* from: https://github.com/KEINOS/mb_levenshtein/blob/master/mb_levenshtein.php
 * MIT License
 * 
 * Copyright (c) 2021 KEINOS and the contributors (https://git.io/J9Gmd)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * mb_levenshtein.
 *
 * Didactic example showing the usage of the previous conversion function.
 * But for better performance, in a real application with a single input string
 * matched against many strings from a database, you will probably want to pre-
 * encode the input only once.
 *
 * @param  string  $s1        One of the strings being evaluated for Levenshtein distance.
 * @param  string  $s2        One of the strings being evaluated for Levenshtein distance.
 * @param  integer $cost_ins  Defines the cost of insertion.
 * @param  integer $cost_rep  Defines the cost of replacement.
 * @param  integer $cost_del  Defines the cost of deletion.
 *
 * @return integer
 */
function keinos_mb_levenshtein($s1, $s2, $cost_ins = 1, $cost_rep = 1, $cost_del = 1)
{
	$charMap = array();
	convert_mb_ascii($s1, $charMap);
	convert_mb_ascii($s2, $charMap);

	return levenshtein($s1, $s2, $cost_ins, $cost_rep, $cost_del);
}

/**
 * convert_mb_ascii.
 *
 * Convert an UTF-8 encoded string to a single-byte string suitable for
 * functions such as levenshtein.
 *
 * The function simply uses (and updates) a tailored dynamic encoding
 * (in/out map parameter) where non-ascii characters are remapped to
 * the range [128-255] in order of appearance.
 *
 * Thus it supports up to 128 different multibyte code points max over
 * the whole set of strings sharing this encoding.
 *
 * @param  string $str  UTF-8 string to be converted to extended ASCII.
 * @param  array  $map  Reference of the map.
 *
 * @return void
 */
function convert_mb_ascii(&$str, &$map)
{
	// find all utf-8 characters
	$matches = array();
	if (! preg_match_all('/[\xC0-\xF7][\x80-\xBF]+/', $str, $matches)) {
		return; // plain ascii string
	}

	// update the encoding map with the characters not already met
	$mapCount = count($map);
	foreach ($matches[0] as $mbc) {
		if (! isset($map[$mbc])) {
			$map[$mbc] = chr(128 + $mapCount);
			$mapCount++;
		}
	}

	// finally remap non-ascii characters
	$str = strtr($str, $map);
}

