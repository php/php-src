--TEST--
Test function posix_ttyname() by substituting argument 1 with object values.
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded('posix')) {
    die('SKIP The posix extension is not loaded.');
}
?>
--FILE--
<?php


echo "*** Test substituting argument 1 with object values ***\n";

class classWithToString
{
        public function __toString() {
                return "Class A object";
        }
}

class classWithoutToString
{
}

$variation_array = array(
  'instance of classWithToString' => new classWithToString(),
  'instance of classWithoutToString' => new classWithoutToString(),
  );


foreach ( $variation_array as $var ) {
  var_dump(posix_ttyname( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with object values ***

Warning: Object of class classWithToString could not be converted to int in %s on line %d
bool(false)

Warning: Object of class classWithoutToString could not be converted to int in %s on line %d
bool(false)
