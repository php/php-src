--TEST--
Test function getservbyname() by substituting argument 1 with object values.
--FILE--
<?php


echo "*** Test substituting argument 1 with object values ***\n";

$protocol = "tcp";


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
  var_dump(getservbyname( $var ,  $protocol ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with object values ***
bool(false)

Warning: getservbyname() expects parameter 1 to be string, object given in %s.php on line %d
NULL
