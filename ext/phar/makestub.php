<?php
$s = str_replace("\r", '', file_get_contents(dirname(__FILE__) . '/shortarc.php'));

$s .= "\nExtract_Phar::go();\n__HALT_COMPILER();";
$news = '';
foreach (token_get_all($s) as $token) {
	if (is_array($token)) {
		if ($token[0] == T_COMMENT) {
			$token[1] = '';
		}
		if ($token[0] == T_WHITESPACE) {
			$n = str_repeat("\n", substr_count($token[1], "\n"));
			$token[1] = strlen($n) ? $n : ' ';
		}
		$news .= $token[1];
	} else {
		$news .= $token;
	}
}
$s = $news . ' ?>';
$slen = strlen($s) - strlen('index.php') - strlen("000");
$s = str_replace('\\', '\\\\', $s);
$s = str_replace('"', '\\"', $s);
$s = str_replace("\n", '\n', $s);
// now we need to find the location of web index file
$webs = substr($s, 0, strpos($s, "000"));
$s = substr($s, strlen($webs) + strlen("000"));
$s1 = substr($s, 0, strpos($s, 'index.php'));
$s2 = substr($s, strlen($s1) + strlen('index.php'));
$s2 = substr($s2, 0, strpos($s2, 'XXXX'));
$s3 = substr($s, strlen($s2) + 4 + strlen($s1) + strlen('index.php'));

$stub = '/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension generated stub         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-' . date('Y') . ' The PHP Group                   |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

static inline void phar_get_stub(const char *index_php, const char *web, size_t *len, char **stub, const int name_len, const int web_len)
{
';
$s1split = str_split($s1, 2046);
$s3split = str_split($s3, 2046);
$took = false;
foreach ($s1split as $i => $chunk) {
	if ($took) {
		$s1split[$i] = substr($chunk, 1);
		$took = false;
	}
	if ($chunk[strlen($chunk) - 1] == '\\') {
		$s1split[$i] .= $s1split[$i + 1][0];
		$took = true;
	}
}
foreach ($s3split as $i => $chunk) {
	if ($took) {
		$s3split[$i] = substr($chunk, 1);
		$took = false;
	}
	if ($chunk[strlen($chunk) - 1] == '\\') {
		$s3split[$i] .= $s3split[$i + 1][0];
		$took = true;
	}
}
$stub .= "\tstatic const char newstub0[] = \"" . $webs . '";
';
foreach ($s1split as $i => $chunk) {
	$s1count = $i + 1;
	$stub .= "\tstatic const char newstub1_" . $i . '[] = "' . $chunk . '";
';
}
$stub .= "\tstatic const char newstub2[] = \"" . $s2 . "\";
";
foreach ($s3split as $i => $chunk) {
	$s3count = $i + 1;
	$stub .= "\tstatic const char newstub3_" . $i . '[] = "' . $chunk . '";
';
}
$stub .= "\n\tstatic const int newstub_len = " . $slen . ";

\t*len = spprintf(stub, name_len + web_len + newstub_len, \"%s%s" . str_repeat('%s', $s1count) . '%s%s%d'
	. str_repeat('%s', $s3count) . '", newstub0, web';
foreach ($s1split as $i => $unused) {
	$stub .= ', newstub1_' . $i;
}
$stub .= ', index_php, newstub2';
$stub .= ", name_len + web_len + newstub_len";
foreach ($s3split as $i => $unused) {
	$stub .= ', newstub3_' . $i;
}
$stub .= ");
}";

file_put_contents(dirname(__FILE__) . '/stub.h', $stub."\n");
?>
