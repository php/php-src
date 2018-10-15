--TEST--
normalizer_get_raw_decomposition()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if( !function_exists( 'normalizer_get_raw_decomposition' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Try getting raw decomposition mappings
 * with procedural and class methods.
 */

function ut_main()
{
	$result = '';
	$strings = [
		"a",
        "\u{FFDA}",
        "\u{FDFA}",
        "",
        "aa",
        "\xF5",
	];

    foreach ($strings as $string) {
        $decomposition = ut_norm_get_raw_decomposition($string, Normalizer::FORM_KC);
        $error_code = intl_get_error_code();
        $error_message = intl_get_error_message();

        $string_hex = bin2hex($string);
        $result .= "---------------------\n";

        if ($decomposition === null) {
            $result .= "'$string_hex' has no decomposition mapping\n" ;
        } else {
            $result .= "'$string_hex' has the decomposition mapping '" . bin2hex($decomposition) . "'\n" ;
        }
        $result .= "error info: '$error_message' ($error_code)\n";
    }

	return $result;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
---------------------
'61' has no decomposition mapping
error info: 'U_ZERO_ERROR' (0)
---------------------
'efbf9a' has the decomposition mapping 'e385a1'
error info: 'U_ZERO_ERROR' (0)
---------------------
'efb7ba' has the decomposition mapping 'd8b5d984d98920d8a7d984d984d98720d8b9d984d98ad98720d988d8b3d984d985'
error info: 'U_ZERO_ERROR' (0)
---------------------
'' has no decomposition mapping
error info: 'Input string must be exactly one UTF-8 encoded code point long.: U_ILLEGAL_ARGUMENT_ERROR' (1)
---------------------
'6161' has no decomposition mapping
error info: 'Input string must be exactly one UTF-8 encoded code point long.: U_ILLEGAL_ARGUMENT_ERROR' (1)
---------------------
'f5' has no decomposition mapping
error info: 'Code point out of range: U_ILLEGAL_ARGUMENT_ERROR' (1)
