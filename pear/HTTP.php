<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2001 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$
//
// HTTP utility functions.
//

if (!empty($GLOBALS['USED_PACKAGES']['HTTP'])) return;
$GLOBALS['USED_PACKAGES']['HTTP'] = true;

class HTTP {
    /**
     * Format a date according to RFC-XXXX (can't remember the HTTP
     * RFC number off-hand anymore, shame on me).  This function
     * honors the "y2k_compliance" php.ini directive.
     *
     * @param $time int UNIX timestamp
     *
     * @return HTTP date string, or false for an invalid timestamp.
     *
     * @author Stig Bakken <ssb@fast.no>
     */
    function Date($time) {
	$y = ini_get("y2k_compliance") ? "Y" : "y";
	return gmdate("D, d M $y H:i:s", $time);
    }

    /**
     * Negotiate language with the user's browser through the
     * Accept-Language HTTP header or the user's host address.
     * Language codes are generally in the form "ll" for a language
     * spoken in only one country, or "ll_CC" for a language spoken in
     * a particular country.  For example, U.S. English is "en_US",
     * while British English is "en_UK".  Portugese as spoken in
     * Portugal is "pt_PT", while Brazilian Portugese is "pt_BR".
     * Two-letter country codes can be found in the ISO 3166 standard.
     *
     * Quantities in the Accept-Language: header are supported, for
     * example:
     *
     *  Accept-Language: en_UK;q=0.7, en_US;q=0.6, no;q=1.0, dk;q=0.8
     *
     * @param $supported an associative array indexed by language
     * codes (country codes) supported by the application.  Values
     * must evaluate to true.
     *
     * @param $default the default language to use if none is found
     * during negotiation, defaults to "en_US" for U.S. English
     *
     * @author Stig Bakken <ssb@fast.no>
     */
    function negotiateLanguage(&$supported, $default = 'en_US') {
	global $HTTP_SERVER_VARS;

	/* If the client has sent an Accept-Language: header, see if
	 * it contains a language we support.
	 */
	if (isset($HTTP_SERVER_VARS['HTTP_ACCEPT_LANGUAGE'])) {
	    $accepted = split(',[[:space:]]*', $HTTP_ACCEPT_LANGUAGE);
	    for ($i = 0; $i < count($accepted); $i++) {
		if (eregi('^([a-z]+);[[:space:]]*q=([0-9\.]+)', $accepted[$i], &$arr)) {
		    $q = (double)$arr[2];
		    $l = $arr[1];
		} else {
		    $q = 42;
		    $l = $accepted[$i];
		}
		if (!empty($supported[$l]) && ($q > 0.0)) {
		    if ($q == 42) {
			return $l;
		    }
		    $candidates[$l] = $q;
		}
	    }
	    if (isset($candidates)) {
		arsort($candidates);
		reset($candidates);
		return key($candidates);
	    }
	}

	/* Check for a valid language code in the top-level domain of
	 * the client's host address.
	 */
	if (ereg("\.[^\.]+$", $HTTP_SERVER_VARS['REMOTE_HOST'], &$arr)) {
	    $lang = strtolower($arr[1]);
	    if (!empty($supported[$lang])) {
		return $lang;
	    }
	}

	return $default;
    }
}
?>
