--TEST--
numfmt_get/set_text_attribute()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Get/set text attribute.
 */


function ut_main()
{
    // Array with data for testing
	$long_str = str_repeat('blah', 100);
    $attributes = array(
        'POSITIVE_PREFIX' => array( NumberFormatter::POSITIVE_PREFIX, '_+_', 12345.1234 ),
        'POSITIVE_SUFFIX' => array( NumberFormatter::POSITIVE_SUFFIX, '_+_', 12345.1234 ),
        'NEGATIVE_PREFIX' => array( NumberFormatter::NEGATIVE_PREFIX, '_-_', -12345.1234 ),
        'NEGATIVE_SUFFIX' => array( NumberFormatter::NEGATIVE_SUFFIX, '_-_', -12345.1234 ),
        'PADDING_CHARACTER' => array( NumberFormatter::PADDING_CHARACTER, '^', 12345.1234 ),
	'POSITIVE_PREFIX-2' => array( NumberFormatter::POSITIVE_PREFIX, $long_str, 12345.1234 ),
//        'CURRENCY_CODE' => array( NumberFormatter::CURRENCY_CODE, '_C_', 12345.1234 )
//        'DEFAULT_RULESET' => array( NumberFormatter::DEFAULT_RULESET, '_DR_', 12345.1234 ),
//        'PUBLIC_RULESETS' => array( NumberFormatter::PUBLIC_RULESETS, '_PR_', 12345.1234 )
    );

    $res_str = '';

    $fmt = ut_nfmt_create( "en_US", NumberFormatter::DECIMAL );

    foreach( $attributes as $attr_name => $data )
    {
        list( $attr, $new_val, $test_number ) = $data;
        $res_str .= "\nAttribute $attr_name\n";

        if( $attr == NumberFormatter::PADDING_CHARACTER )
           ut_nfmt_set_attribute( $fmt, NumberFormatter::FORMAT_WIDTH, 21 );

        // Get default attribute's value
        $def_val = ut_nfmt_get_text_attribute( $fmt, $attr );
        if( $def_val === false )
            $res_str .= "get_text_attribute() error: " . ut_nfmt_get_error_message( $fmt ) . "\n";

        $res_str .= "Default value: [$def_val]\n";
        $res_str .=  "Formatting number with default value: " . ut_nfmt_format( $fmt, $test_number ) . "\n";

        // Set new attribute's value and see if it works out.
        $res_val = ut_nfmt_set_text_attribute( $fmt, $attr, $new_val );
        if( !$res_val )
            $res_str .= "set_text_attribute() error: " . ut_nfmt_get_error_message( $fmt ) . "\n";

        // Get attribute value back.
        $new_val_check = ut_nfmt_get_text_attribute( $fmt, $attr );
        $res_str .=  "New value: [$new_val_check]\n";
        $res_str .=  "Formatting number with new value: " . ut_nfmt_format( $fmt, $test_number ) . "\n";

        // Check if the new value has been set.
        if( $new_val !== $new_val_check )
            $res_str .= "ERROR: New $attr_name symbol value has not been set correctly.\n";

        // Restore attribute's value to default
        ut_nfmt_set_text_attribute( $fmt, $attr, $def_val );

        if( $attr == NumberFormatter::PADDING_CHARACTER )
           ut_nfmt_set_attribute( $fmt, NumberFormatter::FORMAT_WIDTH, 0 );
    }

    //
    $fmt = ut_nfmt_create( "uk_UA", NumberFormatter::CURRENCY );
    $res_str .= sprintf( "\nCurrency ISO-code for locale 'uk_UA' is: %s\n",
                           ut_nfmt_get_text_attribute( $fmt, NumberFormatter::CURRENCY_CODE ) );

    return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
Attribute POSITIVE_PREFIX
Default value: []
Formatting number with default value: 12,345.123
New value: [_+_]
Formatting number with new value: _+_12,345.123

Attribute POSITIVE_SUFFIX
Default value: []
Formatting number with default value: 12,345.123
New value: [_+_]
Formatting number with new value: 12,345.123_+_

Attribute NEGATIVE_PREFIX
Default value: [-]
Formatting number with default value: -12,345.123
New value: [_-_]
Formatting number with new value: _-_12,345.123

Attribute NEGATIVE_SUFFIX
Default value: []
Formatting number with default value: -12,345.123
New value: [_-_]
Formatting number with new value: -12,345.123_-_

Attribute PADDING_CHARACTER
Default value: [*]
Formatting number with default value: ***********12,345.123
New value: [^]
Formatting number with new value: ^^^^^^^^^^^12,345.123

Attribute POSITIVE_PREFIX-2
Default value: []
Formatting number with default value: 12,345.123
New value: [blahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblah]
Formatting number with new value: blahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblah12,345.123

Currency ISO-code for locale 'uk_UA' is: UAH


