--TEST--
Test ResourceBundle::__construct() - existing/missing bundles/locales
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
	
include "resourcebundle.inc";

function ut_main() {
	$str_res = '';
	// all fine
	$r1 = ut_resourcebundle_create( 'root', BUNDLE );
	$str_res .= debug( $r1 );
	$str_res .= print_r( $r1['teststring'], true)."\n";

	// non-root one
	$r1 = ut_resourcebundle_create( 'es', BUNDLE );
	$str_res .= debug( $r1 );
	$str_res .= print_r( $r1['teststring'], true)."\n";

	// fall back
	$r1 = ut_resourcebundle_create( 'en_US', BUNDLE );
        $str_res .= debug( $r1 );
	$str_res .= print_r( $r1['testsring'], true);

	// fall out
	$r2 = ut_resourcebundle_create( 'en_US', BUNDLE, false );
        $str_res .= debug( $r2 );

	// missing
	$r3 = ut_resourcebundle_create( 'en_US', 'nonexisting' );
        $str_res .= debug( $r3 );
	
	return $str_res;
}

	include_once( 'ut_common.inc' );
	ut_run();
?>
--EXPECTF--
ResourceBundle Object
(
)

    0: U_ZERO_ERROR
Hello World!
ResourceBundle Object
(
)

    0: U_ZERO_ERROR
Hola Mundo!
ResourceBundle Object
(
)

 -127: U_USING_DEFAULT_WARNING
NULL
 -127: resourcebundle_ctor: Cannot load libICU resource '%s/resourcebundle' without fallback from en_US to root: U_USING_DEFAULT_WARNING
NULL
    2: resourcebundle_ctor: Cannot load libICU resource bundle: U_MISSING_RESOURCE_ERROR
