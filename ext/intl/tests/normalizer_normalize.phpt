--TEST--
normalize()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
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
		Normalizer::FORM_C,
		Normalizer::FORM_D,
		Normalizer::FORM_KC,
		Normalizer::FORM_KD,
		Normalizer::NONE,
	);

	$forms_str = array (
		Normalizer::FORM_C => 'UNORM_FORM_C',
		Normalizer::FORM_D => 'UNORM_FORM_D',
		Normalizer::FORM_KC => 'UNORM_FORM_KC',
		Normalizer::FORM_KD => 'UNORM_FORM_KD',
		Normalizer::NONE => 'UNORM_NONE',
	);

	/* just make sure all the form constants are defined as in the api spec */
	if ( Normalizer::FORM_C != Normalizer::NFC ||
		 Normalizer::FORM_D != Normalizer::NFD ||
		 Normalizer::FORM_KC != Normalizer::NFKC ||
		 Normalizer::FORM_KD != Normalizer::NFKD ||
		 Normalizer::NONE == Normalizer::FORM_C ) {

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
			$str_norm = ut_norm_normalize( $str, $form );
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
'ABC' normalized to form 'UNORM_FORM_C' is 'ABC'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_C'? = yes	error info: 'U_ZERO_ERROR' (0)
'%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6' normalized to form 'UNORM_FORM_C' is '%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_C'? = yes	error info: 'U_ZERO_ERROR' (0)
'%E2%84%AB%7C%7C%C3%85%7C%7CA%CC%8A' normalized to form 'UNORM_FORM_C' is '%C3%85%7C%7C%C3%85%7C%7C%C3%85'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_C'? = no	error info: 'U_ZERO_ERROR' (0)
'%E2%84%A6%7C%7C%CE%A9' normalized to form 'UNORM_FORM_C' is '%CE%A9%7C%7C%CE%A9'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_C'? = no	error info: 'U_ZERO_ERROR' (0)
'%EF%AC%81' normalized to form 'UNORM_FORM_C' is '%EF%AC%81'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_C'? = yes	error info: 'U_ZERO_ERROR' (0)
'%E1%BA%9B' normalized to form 'UNORM_FORM_C' is '%E1%BA%9B'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_C'? = yes	error info: 'U_ZERO_ERROR' (0)
'ABC' normalized to form 'UNORM_FORM_D' is 'ABC'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_D'? = yes	error info: 'U_ZERO_ERROR' (0)
'%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6' normalized to form 'UNORM_FORM_D' is 'a%CC%88%7C%7Ca%CC%8A%7C%7Co%CC%88'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_D'? = no	error info: 'U_ZERO_ERROR' (0)
'%E2%84%AB%7C%7C%C3%85%7C%7CA%CC%8A' normalized to form 'UNORM_FORM_D' is 'A%CC%8A%7C%7CA%CC%8A%7C%7CA%CC%8A'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_D'? = no	error info: 'U_ZERO_ERROR' (0)
'%E2%84%A6%7C%7C%CE%A9' normalized to form 'UNORM_FORM_D' is '%CE%A9%7C%7C%CE%A9'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_D'? = no	error info: 'U_ZERO_ERROR' (0)
'%EF%AC%81' normalized to form 'UNORM_FORM_D' is '%EF%AC%81'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_D'? = yes	error info: 'U_ZERO_ERROR' (0)
'%E1%BA%9B' normalized to form 'UNORM_FORM_D' is '%C5%BF%CC%87'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_D'? = no	error info: 'U_ZERO_ERROR' (0)
'ABC' normalized to form 'UNORM_FORM_KC' is 'ABC'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC'? = yes	error info: 'U_ZERO_ERROR' (0)
'%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6' normalized to form 'UNORM_FORM_KC' is '%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC'? = yes	error info: 'U_ZERO_ERROR' (0)
'%E2%84%AB%7C%7C%C3%85%7C%7CA%CC%8A' normalized to form 'UNORM_FORM_KC' is '%C3%85%7C%7C%C3%85%7C%7C%C3%85'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC'? = no	error info: 'U_ZERO_ERROR' (0)
'%E2%84%A6%7C%7C%CE%A9' normalized to form 'UNORM_FORM_KC' is '%CE%A9%7C%7C%CE%A9'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC'? = no	error info: 'U_ZERO_ERROR' (0)
'%EF%AC%81' normalized to form 'UNORM_FORM_KC' is 'fi'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC'? = no	error info: 'U_ZERO_ERROR' (0)
'%E1%BA%9B' normalized to form 'UNORM_FORM_KC' is '%E1%B9%A1'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KC'? = no	error info: 'U_ZERO_ERROR' (0)
'ABC' normalized to form 'UNORM_FORM_KD' is 'ABC'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KD'? = yes	error info: 'U_ZERO_ERROR' (0)
'%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6' normalized to form 'UNORM_FORM_KD' is 'a%CC%88%7C%7Ca%CC%8A%7C%7Co%CC%88'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KD'? = no	error info: 'U_ZERO_ERROR' (0)
'%E2%84%AB%7C%7C%C3%85%7C%7CA%CC%8A' normalized to form 'UNORM_FORM_KD' is 'A%CC%8A%7C%7CA%CC%8A%7C%7CA%CC%8A'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KD'? = no	error info: 'U_ZERO_ERROR' (0)
'%E2%84%A6%7C%7C%CE%A9' normalized to form 'UNORM_FORM_KD' is '%CE%A9%7C%7C%CE%A9'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KD'? = no	error info: 'U_ZERO_ERROR' (0)
'%EF%AC%81' normalized to form 'UNORM_FORM_KD' is 'fi'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KD'? = no	error info: 'U_ZERO_ERROR' (0)
'%E1%BA%9B' normalized to form 'UNORM_FORM_KD' is 's%CC%87'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_FORM_KD'? = no	error info: 'U_ZERO_ERROR' (0)
'ABC' normalized to form 'UNORM_NONE' is 'ABC'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_NONE'? = no	error info: 'normalizer_normalize: illegal normalization form: U_ILLEGAL_ARGUMENT_ERROR' (1)
'%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6' normalized to form 'UNORM_NONE' is '%C3%A4%7C%7C%C3%A5%7C%7C%C3%B6'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_NONE'? = no	error info: 'normalizer_normalize: illegal normalization form: U_ILLEGAL_ARGUMENT_ERROR' (1)
'%E2%84%AB%7C%7C%C3%85%7C%7CA%CC%8A' normalized to form 'UNORM_NONE' is '%E2%84%AB%7C%7C%C3%85%7C%7CA%CC%8A'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_NONE'? = no	error info: 'normalizer_normalize: illegal normalization form: U_ILLEGAL_ARGUMENT_ERROR' (1)
'%E2%84%A6%7C%7C%CE%A9' normalized to form 'UNORM_NONE' is '%E2%84%A6%7C%7C%CE%A9'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_NONE'? = no	error info: 'normalizer_normalize: illegal normalization form: U_ILLEGAL_ARGUMENT_ERROR' (1)
'%EF%AC%81' normalized to form 'UNORM_NONE' is '%EF%AC%81'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_NONE'? = no	error info: 'normalizer_normalize: illegal normalization form: U_ILLEGAL_ARGUMENT_ERROR' (1)
'%E1%BA%9B' normalized to form 'UNORM_NONE' is '%E1%BA%9B'	error info: 'U_ZERO_ERROR' (0)
		is in form 'UNORM_NONE'? = no	error info: 'normalizer_normalize: illegal normalization form: U_ILLEGAL_ARGUMENT_ERROR' (1)
