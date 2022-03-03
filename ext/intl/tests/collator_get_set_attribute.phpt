--TEST--
get/set_attribute()
--EXTENSIONS--
intl
--FILE--
<?php

/*
 * Try to set/get a collation attribute.
 */


/*
 * Return current normalication mode.
 */
function check_val( $coll )
{
    $val = ut_coll_get_attribute( $coll, Collator::NORMALIZATION_MODE );
    return sprintf( "%s\n", ( $val == Collator::OFF ? "off" : "on" ) );
}

function ut_main()
{
    $res = '';
    $coll = ut_coll_create( 'en_US' );

    ut_coll_set_attribute( $coll, Collator::NORMALIZATION_MODE, Collator::OFF );
    $res .= check_val( $coll );

    ut_coll_set_attribute( $coll, Collator::NORMALIZATION_MODE, Collator::ON );
    $res .= check_val( $coll );

    return $res;
}

include( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
off
on
