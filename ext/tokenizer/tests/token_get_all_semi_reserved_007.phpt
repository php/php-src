--TEST--
Semi reserved words support: nested classes
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
$tokens = token_get_all('<?php
  return new class {
    function function() { // T_STRING
      function function(){} // T_FUNCTION
      return new class extends {
        function function(){ // T_STRING
          function function(){} // T_FUNCTION
          return new class {
            function function(){ // T_STRING
              function function(){} // T_FUNCTION
            }
          }
        }
      }
    }
  }
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

L2: T_RETURN return
L2: T_NEW new
L2: T_CLASS class
{
L3: T_FUNCTION function
L3: T_STRING function
(
)
{
L3: T_COMMENT // T_STRING

L4: T_FUNCTION function
L4: T_FUNCTION function
(
)
{
}
L4: T_COMMENT // T_FUNCTION

L5: T_RETURN return
L5: T_NEW new
L5: T_CLASS class
L5: T_EXTENDS extends
{
L6: T_FUNCTION function
L6: T_STRING function
(
)
{
L6: T_COMMENT // T_STRING

L7: T_FUNCTION function
L7: T_FUNCTION function
(
)
{
}
L7: T_COMMENT // T_FUNCTION

L8: T_RETURN return
L8: T_NEW new
L8: T_CLASS class
{
L9: T_FUNCTION function
L9: T_STRING function
(
)
{
L9: T_COMMENT // T_STRING

L10: T_FUNCTION function
L10: T_FUNCTION function
(
)
{
}
L10: T_COMMENT // T_FUNCTION

}
}
}
}
}
}
Done
