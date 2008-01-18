<?php
$s = str_replace("\r", '', file_get_contents(dirname(__FILE__) . '/shortarc.php'));
$s .= "\nExtract_Phar::go(XXXX);\n__HALT_COMPILER();";
$news = '';
$last = -1;
foreach (token_get_all($s) as $token) {
    if (is_array($token)) {
        if ($token[0] == T_WHITESPACE) {
            if ($last == T_COMMENT) {
                $token[1] = '';
            } else {
                $n = str_repeat("\n", substr_count($token[1], "\n"));
                $token[1] = strlen($n) ? $n : ' ';
            }
        }
        $last = $token[0];
        $news .= $token[1];
    } else {
        $news .= $token;
    }
}
$s = $news . ' ?>';
$s = str_replace('XXXX', strlen($s)-1, $s);
$s = str_replace('"', '\\"', $s);
$s = str_replace("\n", '\n', $s);

$stub = '/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension generated stub         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-' . date('Y') . ' The PHP Group                                |
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

static const char newstub[] = "' . $s . '";
';

file_put_contents(dirname(__FILE__) . '/stub.h', $stub);
?>