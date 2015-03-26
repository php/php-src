--TEST--
Semi reserved words support: methods
--FILE--
<?php
$tokens = token_get_all('<?php
  class SomeClass {
    function function(){
      function function(){}
    }
  }
  (new Class)->function();

  function function(){}
');

array_walk($tokens, function($tk) {
  if(is_array($tk)) {
    if(($t = token_name($tk[0])) == 'T_WHITESPACE') return;
    echo "L{$tk[2]}: ".$t." {$tk[1]}", PHP_EOL;
  }
  else echo $tk, PHP_EOL;
});

echo "Done";

?>
--EXPECTF--
L1: T_OPEN_TAG <?php

L2: T_CLASS class
L2: T_STRING SomeClass
{
L3: T_FUNCTION function
L3: T_STRING function
(
)
{
L4: T_FUNCTION function
L4: T_FUNCTION function
(
)
{
}
}
}
(
L7: T_NEW new
L7: T_CLASS Class
)
L7: T_OBJECT_OPERATOR ->
L7: T_STRING function
(
)
;
L9: T_FUNCTION function
L9: T_FUNCTION function
(
)
{
}
Done
