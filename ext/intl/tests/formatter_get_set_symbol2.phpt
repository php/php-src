--TEST--
numfmt_get/set_symbol() icu >= 4.8
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.8') < 0) print 'skip'; ?>
<?php if (PHP_INT_SIZE != 8) die('skip 64-bit only'); ?>
--FILE--
<?php

/*
 * Get/set symbol.
 */


function ut_main()
{
	$longstr = str_repeat("blah", 10);
    $symbols = array(
        'DECIMAL_SEPARATOR_SYMBOL' => array( NumberFormatter::DECIMAL_SEPARATOR_SYMBOL, '_._', 12345.123456, NumberFormatter::DECIMAL ),
        'GROUPING_SEPARATOR_SYMBOL' => array( NumberFormatter::GROUPING_SEPARATOR_SYMBOL, '_,_', 12345.123456, NumberFormatter::DECIMAL ),
        'PATTERN_SEPARATOR_SYMBOL' => array( NumberFormatter::PATTERN_SEPARATOR_SYMBOL, '_;_', 12345.123456, NumberFormatter::DECIMAL ),
        'PERCENT_SYMBOL' => array( NumberFormatter::PERCENT_SYMBOL, '_%_', 12345.123456, NumberFormatter::PERCENT ),
        'ZERO_DIGIT_SYMBOL' => array( NumberFormatter::ZERO_DIGIT_SYMBOL, '_ZD_', 12345.123456, NumberFormatter::DECIMAL ),
        'DIGIT_SYMBOL' => array( NumberFormatter::DIGIT_SYMBOL, '_DS_', 12345.123456, NumberFormatter::DECIMAL ),
        'MINUS_SIGN_SYMBOL' => array( NumberFormatter::MINUS_SIGN_SYMBOL, '_-_', -12345.123456, NumberFormatter::DECIMAL ),
        'PLUS_SIGN_SYMBOL' => array( NumberFormatter::PLUS_SIGN_SYMBOL, '_+_', 12345.123456, NumberFormatter::SCIENTIFIC ),
        'CURRENCY_SYMBOL' => array( NumberFormatter::CURRENCY_SYMBOL, '_$_', 12345.123456, NumberFormatter::CURRENCY ),
        'INTL_CURRENCY_SYMBOL' => array( NumberFormatter::INTL_CURRENCY_SYMBOL, '_$_', 12345.123456, NumberFormatter::CURRENCY ),
        'MONETARY_SEPARATOR_SYMBOL' => array( NumberFormatter::MONETARY_SEPARATOR_SYMBOL, '_MS_', 12345.123456, NumberFormatter::CURRENCY ),
        'EXPONENTIAL_SYMBOL' => array( NumberFormatter::EXPONENTIAL_SYMBOL, '_E_', 12345.123456, NumberFormatter::SCIENTIFIC ),
        'PERMILL_SYMBOL' => array( NumberFormatter::PERMILL_SYMBOL, '_PS_', 12345.123456, NumberFormatter::DECIMAL ),
        'PAD_ESCAPE_SYMBOL' => array( NumberFormatter::PAD_ESCAPE_SYMBOL, '_PE_', 12345.123456, NumberFormatter::DECIMAL ),
        'INFINITY_SYMBOL' => array( NumberFormatter::INFINITY_SYMBOL, '_IS_', 12345.123456, NumberFormatter::DECIMAL ),
        'NAN_SYMBOL' => array( NumberFormatter::NAN_SYMBOL, '_N_', 12345.123456, NumberFormatter::DECIMAL ),
        'SIGNIFICANT_DIGIT_SYMBOL' => array( NumberFormatter::SIGNIFICANT_DIGIT_SYMBOL, '_SD_', 12345.123456, NumberFormatter::DECIMAL ),
        'MONETARY_GROUPING_SEPARATOR_SYMBOL' => array( NumberFormatter::MONETARY_GROUPING_SEPARATOR_SYMBOL, '_MG_', 12345.123456, NumberFormatter::CURRENCY ),
	'MONETARY_GROUPING_SEPARATOR_SYMBOL-2' => array( NumberFormatter::MONETARY_GROUPING_SEPARATOR_SYMBOL, "&nbsp;", 12345.123456, NumberFormatter::CURRENCY ),
	'MONETARY_GROUPING_SEPARATOR_SYMBOL-3' => array( NumberFormatter::MONETARY_GROUPING_SEPARATOR_SYMBOL, $longstr, 12345.123456, NumberFormatter::CURRENCY ),
    );

    $res_str = '';

    foreach( $symbols as $symb_name => $data )
    {
        list( $symb, $new_val, $number, $attr ) = $data;

        $fmt = ut_nfmt_create( 'en_US', $attr);

        $res_str .= "\nSymbol '$symb_name'\n";

        // Get original symbol value.
        $orig_val = ut_nfmt_get_symbol( $fmt, $symb );
        $res_str .= "Default symbol: [$orig_val]\n";

        // Set a new symbol value.
        $res_val = ut_nfmt_set_symbol( $fmt, $symb, $new_val );
        if( !$res_val )
            $res_str .= "set_symbol() error: " . ut_nfmt_get_error_message( $fmt ) . "\n";

        // Get the symbol value back.
        $new_val_check = ut_nfmt_get_symbol( $fmt, $symb );
        if( !$new_val_check )
            $res_str .= "get_symbol() error: " . ut_nfmt_get_error_message( $fmt ) . "\n";

        $res_str .= "New symbol: [$new_val_check]\n";

        // Check if the new value has been set.
        if( $new_val_check !== $new_val )
            $res_str .= "ERROR: New $symb_name symbol value has not been set correctly.\n";

        // Format the number using the new value.
        $s = ut_nfmt_format( $fmt, $number );
        $res_str .= "A number formatted with the new symbol: $s\n";

        // Restore attribute's symbol.
        ut_nfmt_set_symbol( $fmt, $symb, $orig_val );
    }
    $badvals = array(2147483648, -2147483648, -1, 4294901761);
    foreach($badvals as $badval) {
	    if(ut_nfmt_get_symbol( $fmt, 2147483648 ))  {
		$res_str .= "Bad value $badval should return false!\n";
	    }
    }
    return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
Symbol 'DECIMAL_SEPARATOR_SYMBOL'
Default symbol: [.]
New symbol: [_._]
A number formatted with the new symbol: 12,345_._123

Symbol 'GROUPING_SEPARATOR_SYMBOL'
Default symbol: [,]
New symbol: [_,_]
A number formatted with the new symbol: 12_,_345.123

Symbol 'PATTERN_SEPARATOR_SYMBOL'
Default symbol: [;]
New symbol: [_;_]
A number formatted with the new symbol: 12,345.123

Symbol 'PERCENT_SYMBOL'
Default symbol: [%]
New symbol: [_%_]
A number formatted with the new symbol: 1,234,512_%_

Symbol 'ZERO_DIGIT_SYMBOL'
Default symbol: [0]
New symbol: [_ZD_]
A number formatted with the new symbol: 12,345.123

Symbol 'DIGIT_SYMBOL'
Default symbol: [#]
New symbol: [_DS_]
A number formatted with the new symbol: 12,345.123

Symbol 'MINUS_SIGN_SYMBOL'
Default symbol: [-]
New symbol: [_-_]
A number formatted with the new symbol: _-_12,345.123

Symbol 'PLUS_SIGN_SYMBOL'
Default symbol: [+]
New symbol: [_+_]
A number formatted with the new symbol: 1.2345123456E4

Symbol 'CURRENCY_SYMBOL'
Default symbol: [$]
New symbol: [_$_]
A number formatted with the new symbol: _$_12,345.12

Symbol 'INTL_CURRENCY_SYMBOL'
Default symbol: [USD]
New symbol: [_$_]
A number formatted with the new symbol: $12,345.12

Symbol 'MONETARY_SEPARATOR_SYMBOL'
Default symbol: [.]
New symbol: [_MS_]
A number formatted with the new symbol: $12,345_MS_12

Symbol 'EXPONENTIAL_SYMBOL'
Default symbol: [E]
New symbol: [_E_]
A number formatted with the new symbol: 1.2345123456_E_4

Symbol 'PERMILL_SYMBOL'
Default symbol: [‰]
New symbol: [_PS_]
A number formatted with the new symbol: 12,345.123

Symbol 'PAD_ESCAPE_SYMBOL'
Default symbol: [*]
New symbol: [_PE_]
A number formatted with the new symbol: 12,345.123

Symbol 'INFINITY_SYMBOL'
Default symbol: [∞]
New symbol: [_IS_]
A number formatted with the new symbol: 12,345.123

Symbol 'NAN_SYMBOL'
Default symbol: [NaN]
New symbol: [_N_]
A number formatted with the new symbol: 12,345.123

Symbol 'SIGNIFICANT_DIGIT_SYMBOL'
Default symbol: [@]
New symbol: [_SD_]
A number formatted with the new symbol: 12,345.123

Symbol 'MONETARY_GROUPING_SEPARATOR_SYMBOL'
Default symbol: [,]
New symbol: [_MG_]
A number formatted with the new symbol: $12_MG_345.12

Symbol 'MONETARY_GROUPING_SEPARATOR_SYMBOL-2'
Default symbol: [,]
New symbol: [&nbsp;]
A number formatted with the new symbol: $12&nbsp;345.12

Symbol 'MONETARY_GROUPING_SEPARATOR_SYMBOL-3'
Default symbol: [,]
New symbol: [blahblahblahblahblahblahblahblahblahblah]
A number formatted with the new symbol: $12blahblahblahblahblahblahblahblahblahblah345.12

