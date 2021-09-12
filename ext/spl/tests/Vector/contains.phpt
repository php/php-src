--TEST--
Vector contains()/indexOf();
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
$o = new stdClass();
$it = new Vector(['first', $o, 'first']);
foreach ([null, 'o', $o, new stdClass(), strtolower('FIRST')] as $value) {
    printf("%s: contains=%s, indexOf=%s\n", json_encode($value), json_encode($it->contains($value)), json_encode($it->indexOf($value)));
}
?>
--EXPECT--
null: contains=false, indexOf=false
"o": contains=false, indexOf=false
{}: contains=true, indexOf=1
{}: contains=false, indexOf=false
"first": contains=true, indexOf=0
