--TEST--
normalize() NFKC_Casefold
--SKIPIF--
<?php if (!extension_loaded('intl')) print 'skip intl extension not loaded'; ?>
<?php if (!defined('Normalizer::FORM_KC_CF')) print 'skip Normalizer::FORM_KC_CF constant not defined'; ?>
--FILE--
<?php

/*
 * Try normalization and test normalization
 * with Procedural and Object methods.
 */

function ut_main()
{
	$res_str = '';

	$forms = array(
		Normalizer::FORM_KC_CF,
	);

	$forms_str = array (
		Normalizer::FORM_KC_CF => 'UNORM_FORM_KC_CF',
	);

	/* just make sure all the form constants are defined as in the api spec */
	if (Normalizer::FORM_C != Normalizer::NFC) {
			$res_str .= "Invalid normalization form declarations!\n";
	}

	$char_a_diaeresis = "\xC3\xA4";	// 'LATIN SMALL LETTER A WITH DIAERESIS' (U+00E4)
	$char_a_ring = "\xC3\xA5";		// 'LATIN SMALL LETTER A WITH RING ABOVE' (U+00E5)
	$char_o_diaeresis = "\xC3\xB6";    // 'LATIN SMALL LETTER O WITH DIAERESIS' (U+00F6)

	$char_angstrom_sign = "\xE2\x84\xAB"; // 'ANGSTROM SIGN' (U+212B)
	$char_A_ring = "\xC3\x85";	// 'LATIN CAPITAL LETTER A WITH RING ABOVE' (U+00C5)

	$char_ohm_sign = "\xE2\x84\xA6";	// 'OHM SIGN' (U+2126)
	$char_omega = "\xCE\xA9";  // 'GREEK CAPITAL LETTER OMEGA' (U+03A9)

	$char_combining_ring_above = "\xCC\x8A";  // 'COMBINING RING ABOVE' (U+030A)

	$char_fi_ligature = "\xEF\xAC\x81";  // 'LATIN SMALL LIGATURE FI' (U+FB01)

	$char_long_s_dot = "\xE1\xBA\x9B";	// 'LATIN SMALL LETTER LONG S WITH DOT ABOVE' (U+1E9B)

	$strs = array(
		'ABC',
		'abc',
		$char_a_diaeresis . '||' . $char_a_ring . '||' . $char_o_diaeresis,
		$char_angstrom_sign . '||' . $char_A_ring . '||' . 'A' . $char_combining_ring_above,
		$char_ohm_sign . '||' . $char_omega,
		$char_fi_ligature,
		$char_long_s_dot,
	);

	foreach( $forms as $form )
	{
		foreach( $strs as $str )
		{
			if (Normalizer::NONE == $form) {
				/* Hide deprecation warning. */
				$str_norm = @ut_norm_normalize( $str, $form );
			} else {
				$str_norm = ut_norm_normalize( $str, $form );
			}
			$error_code = intl_get_error_code();
			$error_message = intl_get_error_message();

			$str_hex = urlencode($str);
			$str_norm_hex = urlencode($str_norm);
			$res_str .= "'$str_hex' normalized to form '{$forms_str[$form]}' is '$str_norm_hex'"
					 .	"\terror info: '$error_message' ($error_code)\n"
					 .	"";

			$is_norm = ut_norm_is_normalized( $str, $form );
			$error_code = intl_get_error_code();
			$error_message = intl_get_error_message();

			$res_str .= "		is in form '{$forms_str[$form]}'? = " . ($is_norm ? "yes" : "no")
					 .	"\terror info: '$error_message' ($error_code)\n"
					 .	"";
		}
	}

	return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
'ABC' normalized to form 'UNORM_FORM_KC_CF' is 'abc'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC_CF'? = no	error info: 'U_ZERO_ERROR' (0)
'abc' normalized to form 'UNORM_FORM_KC_CF' is 'abc'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC_CF'? = yes	error info: 'U_ZERO_ERROR' (0)
'%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6' normalized to form 'UNORM_FORM_KC_CF' is '%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC_CF'? = yes	error info: 'U_ZERO_ERROR' (0)
'%E2%84%AB%7C%7C%C3%85%7C%7CA%CC%8A' normalized to form 'UNORM_FORM_KC_CF' is '%C3%A5%7C%7C%C3%A5%7C%7C%C3%A5'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC_CF'? = no	error info: 'U_ZERO_ERROR' (0)
'%E2%84%A6%7C%7C%CE%A9' normalized to form 'UNORM_FORM_KC_CF' is '%CF%89%7C%7C%CF%89'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC_CF'? = no	error info: 'U_ZERO_ERROR' (0)
'%EF%AC%81' normalized to form 'UNORM_FORM_KC_CF' is 'fi'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC_CF'? = no	error info: 'U_ZERO_ERROR' (0)
'%E1%BA%9B' normalized to form 'UNORM_FORM_KC_CF' is '%E1%B9%A1'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC_CF'? = no	error info: 'U_ZERO_ERROR' (0)
