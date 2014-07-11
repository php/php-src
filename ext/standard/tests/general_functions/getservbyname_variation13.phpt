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

Warning: getservbyname() expects parameter 2 to be string, object given in %s.php on line %d
NULL
