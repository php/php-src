--TEST--
numfmt_get/set_attribute()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.2', '<') != 1) print 'skip for ICU 4.4+'; ?>
--FILE--
<?php

/*
 * Get/set various number formatting attributes.
 */


function ut_main()
{
    // attr_name => array( attr, value )
    $attributes = array(
        'PARSE_INT_ONLY' => array( NumberFormatter::PARSE_INT_ONLY, 1, 12345.123456 ),
        'GROUPING_USED' => array( NumberFormatter::GROUPING_USED, 0, 12345.123456 ),
        'DECIMAL_ALWAYS_SHOWN' => array( NumberFormatter::DECIMAL_ALWAYS_SHOWN, 1, 12345 ),
        'MAX_INTEGER_DIGITS' => array( NumberFormatter::MAX_INTEGER_DIGITS, 2, 12345.123456 ),
        'MIN_INTEGER_DIGITS' => array( NumberFormatter::MIN_INTEGER_DIGITS, 20, 12345.123456 ),
        'INTEGER_DIGITS' => array( NumberFormatter::INTEGER_DIGITS, 7, 12345.123456 ),
        'MAX_FRACTION_DIGITS' => array( NumberFormatter::MAX_FRACTION_DIGITS, 2, 12345.123456 ),
        'MIN_FRACTION_DIGITS' => array( NumberFormatter::MIN_FRACTION_DIGITS, 20, 12345.123456 ),
        'FRACTION_DIGITS' => array( NumberFormatter::FRACTION_DIGITS, 5, 12345.123456 ),
        'MULTIPLIER' => array( NumberFormatter::MULTIPLIER, 2, 12345.123456 ),
        'GROUPING_SIZE' => array( NumberFormatter::GROUPING_SIZE, 2, 12345.123456 ),
        'ROUNDING_MODE' => array( NumberFormatter::ROUNDING_MODE, 1, 12345.123456 ),
        'ROUNDING_INCREMENT' => array( NumberFormatter::ROUNDING_INCREMENT, (float)2, 12345.123456 ),
        'FORMAT_WIDTH' => array( NumberFormatter::FORMAT_WIDTH, 27, 12345.123456 ),
        'PADDING_POSITION' => array( NumberFormatter::PADDING_POSITION, 2, 12345.123456 ),
        'SECONDARY_GROUPING_SIZE' => array( NumberFormatter::SECONDARY_GROUPING_SIZE, 2, 12345.123456 ),
        'SIGNIFICANT_DIGITS_USED' => array( NumberFormatter::SIGNIFICANT_DIGITS_USED, 1, 12345.123456 ),
        'MIN_SIGNIFICANT_DIGITS' => array( NumberFormatter::MIN_SIGNIFICANT_DIGITS, 3, 1 ),
        'MAX_SIGNIFICANT_DIGITS' => array( NumberFormatter::MAX_SIGNIFICANT_DIGITS, 4, 12345.123456 ),
        // 'LENIENT_PARSE' => array( NumberFormatter::LENIENT_PARSE, 2, 12345.123456 )
    );

    $res_str = '';

    $fmt = ut_nfmt_create( "en_US", NumberFormatter::DECIMAL );

    foreach( $attributes as $attr_name => $args )
    {
        list( $attr, $new_val, $number ) = $args;
        $res_str .= "\nAttribute $attr_name\n";

        // Get original value of the attribute.
        $orig_val = ut_nfmt_get_attribute( $fmt, $attr );

        // Format the number using the original attribute value.
        $rc = ut_nfmt_format( $fmt, $number );

        $ps = ut_nfmt_parse( $fmt, $rc );

        $res_str .= sprintf( "Old attribute value: %s ;  Format result: %s ; Parse result: %s\n",
                             dump( $orig_val ),
                             dump( $rc ),
                             dump( $ps ) );

        // Set new attribute value.
        $rc = ut_nfmt_set_attribute( $fmt, $attr, $new_val );
        if( $rc )
            $res_str .= "Setting attribute: ok\n";
        else
            $res_str .= sprintf( "Setting attribute failed: %s\n", ut_nfmt_get_error_message( $fmt ) );

        // Format the number using the new value.
        $rc = ut_nfmt_format( $fmt, $number );

        // Get current value of the attribute and check if it equals $new_val.
        $attr_val_check = ut_nfmt_get_attribute( $fmt, $attr );
        if( $attr_val_check !== $new_val )
            $res_str .= "ERROR: New $attr_name attribute value has not been set correctly.\n";

        $ps = ut_nfmt_parse( $fmt, $rc );

        $res_str .= sprintf( "New attribute value: %s ;  Format result: %s ; Parse result: %s\n",
                             dump( $new_val ),
                             dump( $rc ),
                             dump( $ps ) );


        // Restore original attribute of the  value
        if( $attr != NumberFormatter::INTEGER_DIGITS && $attr != NumberFormatter::FRACTION_DIGITS
             && $attr != NumberFormatter::FORMAT_WIDTH && $attr != NumberFormatter::SIGNIFICANT_DIGITS_USED )
            ut_nfmt_set_attribute( $fmt, $attr, $orig_val );
    }

    return $res_str;
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();

?>
--EXPECT--
Attribute PARSE_INT_ONLY
Old attribute value: 0 ;  Format result: '12,345.123' ; Parse result: 12345.123
Setting attribute: ok
New attribute value: 1 ;  Format result: '12,345.123' ; Parse result: 12345

Attribute GROUPING_USED
Old attribute value: 1 ;  Format result: '12,345.123' ; Parse result: 12345.123
Setting attribute: ok
New attribute value: 0 ;  Format result: '12345.123' ; Parse result: 12345.123

Attribute DECIMAL_ALWAYS_SHOWN
Old attribute value: 0 ;  Format result: '12,345' ; Parse result: 12345
Setting attribute: ok
New attribute value: 1 ;  Format result: '12,345.' ; Parse result: 12345

Attribute MAX_INTEGER_DIGITS
Old attribute value: 309 ;  Format result: '12,345.123' ; Parse result: 12345.123
Setting attribute: ok
New attribute value: 2 ;  Format result: '45.123' ; Parse result: 45.123

Attribute MIN_INTEGER_DIGITS
Old attribute value: 1 ;  Format result: '12,345.123' ; Parse result: 12345.123
Setting attribute: ok
New attribute value: 20 ;  Format result: '00,000,000,000,000,012,345.123' ; Parse result: 12345.123

Attribute INTEGER_DIGITS
Old attribute value: 1 ;  Format result: '12,345.123' ; Parse result: 12345.123
Setting attribute: ok
New attribute value: 7 ;  Format result: '0,012,345.123' ; Parse result: 12345.123

Attribute MAX_FRACTION_DIGITS
Old attribute value: 3 ;  Format result: '0,012,345.123' ; Parse result: 12345.123
Setting attribute: ok
New attribute value: 2 ;  Format result: '0,012,345.12' ; Parse result: 12345.12

Attribute MIN_FRACTION_DIGITS
Old attribute value: 0 ;  Format result: '0,012,345.123' ; Parse result: 12345.123
Setting attribute: ok
New attribute value: 20 ;  Format result: '0,012,345.12345600000000000000' ; Parse result: 12345.123456

Attribute FRACTION_DIGITS
Old attribute value: 0 ;  Format result: '0,012,345.123456' ; Parse result: 12345.123456
Setting attribute: ok
New attribute value: 5 ;  Format result: '0,012,345.12346' ; Parse result: 12345.12346

Attribute MULTIPLIER
Old attribute value: 1 ;  Format result: '0,012,345.12346' ; Parse result: 12345.12346
Setting attribute: ok
New attribute value: 2 ;  Format result: '0,024,690.24691' ; Parse result: 12345.123455

Attribute GROUPING_SIZE
Old attribute value: 3 ;  Format result: '0,012,345.12346' ; Parse result: 12345.12346
Setting attribute: ok
New attribute value: 2 ;  Format result: '0,01,23,45.12346' ; Parse result: 12345.12346

Attribute ROUNDING_MODE
Old attribute value: 4 ;  Format result: '0,012,345.12346' ; Parse result: 12345.12346
Setting attribute: ok
New attribute value: 1 ;  Format result: '0,012,345.12345' ; Parse result: 12345.12345

Attribute ROUNDING_INCREMENT
Old attribute value: 0 ;  Format result: '0,012,345.12346' ; Parse result: 12345.12346
Setting attribute: ok
New attribute value: 2 ;  Format result: '0,012,346.00000' ; Parse result: 12346

Attribute FORMAT_WIDTH
Old attribute value: 0 ;  Format result: '0,012,345.12346' ; Parse result: 12345.12346
Setting attribute: ok
New attribute value: 27 ;  Format result: '************0,012,345.12346' ; Parse result: 12345.12346

Attribute PADDING_POSITION
Old attribute value: 0 ;  Format result: '************0,012,345.12346' ; Parse result: 12345.12346
Setting attribute: ok
New attribute value: 2 ;  Format result: '0,012,345.12346************' ; Parse result: 12345.12346

Attribute SECONDARY_GROUPING_SIZE
Old attribute value: 0 ;  Format result: '************0,012,345.12346' ; Parse result: 12345.12346
Setting attribute: ok
New attribute value: 2 ;  Format result: '************00,12,345.12346' ; Parse result: 12345.12346

Attribute SIGNIFICANT_DIGITS_USED
Old attribute value: 0 ;  Format result: '************0,012,345.12346' ; Parse result: 12345.12346
Setting attribute: ok
New attribute value: 1 ;  Format result: '*******************12,345.1' ; Parse result: 12345.1

Attribute MIN_SIGNIFICANT_DIGITS
Old attribute value: 1 ;  Format result: '**************************1' ; Parse result: 1
Setting attribute: ok
New attribute value: 3 ;  Format result: '***********************1.00' ; Parse result: 1

Attribute MAX_SIGNIFICANT_DIGITS
Old attribute value: 6 ;  Format result: '*******************12,345.1' ; Parse result: 12345.1
Setting attribute: ok
New attribute value: 4 ;  Format result: '*********************12,350' ; Parse result: 12350
