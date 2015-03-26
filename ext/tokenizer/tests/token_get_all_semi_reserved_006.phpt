--TEST--
Semi reserved words support: string interpolation
--FILE--
<?php
$tokens = token_get_all('<?php
  class SomeClass {
    const STRING = "{$foo} ${bar}"; // invalid code but hould be tokenized right
    function function() {
        return "{$foo} ${bar}";
    }
  }
  function function(){} // outside class
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
L3: T_CONST const
L3: T_STRING STRING
=
"
L3: T_CURLY_OPEN {
L3: T_VARIABLE $foo
}
L3: T_ENCAPSED_AND_WHITESPACE  
L3: T_DOLLAR_OPEN_CURLY_BRACES ${
L3: T_STRING_VARNAME bar
}
"
;
L3: T_COMMENT // invalid code but hould be tokenized right

L4: T_FUNCTION function
L4: T_STRING function
(
)
{
L5: T_RETURN return
"
L5: T_CURLY_OPEN {
L5: T_VARIABLE $foo
}
L5: T_ENCAPSED_AND_WHITESPACE  
L5: T_DOLLAR_OPEN_CURLY_BRACES ${
L5: T_STRING_VARNAME bar
}
"
;
}
}
L8: T_FUNCTION function
L8: T_FUNCTION function
(
)
{
}
L8: T_COMMENT // outside class

Done
