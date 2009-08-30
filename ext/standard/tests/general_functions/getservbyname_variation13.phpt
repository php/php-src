--TEST--
Test function getservbyname() by substituting argument 2 with object values.
--FILE--
<?php


echo "*** Test substituting argument 2 with object values ***\n";

$service = "www";


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
  var_dump(getservbyname( $service, $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 2 with object values ***
bool(false)

Catchable fatal error: Object of class classWithoutToString could not be converted to string in %s.php on line %d
