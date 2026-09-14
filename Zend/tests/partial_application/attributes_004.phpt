--TEST--
PFA attribute UAF
--CREDITS--
Ryan @ Calif.io
--FILE--
<?php

/* Reproduces with opcache.enable_cli=0. Not adding this to the INI section so
 * that it's tested in all configurations. */

#[NoDiscard(ND_MESSAGE)]
function userTarget(mixed $value): mixed
{
    return $value;
}

define('ND_MESSAGE', str_repeat('message-', 32).time());

function arguments(ReflectionFunction $function): array
{
    return $function->getAttributes(NoDiscard::class)[0]->getArguments();
}

$original = new ReflectionFunction('userTarget');
var_dump(arguments($original));

$partial = userTarget(?);
echo "partial-created\n";
var_dump(arguments($original));
var_dump(arguments(new ReflectionFunction($partial)));

unset($partial);
gc_collect_cycles();
echo "partial-destroyed\n";
var_dump(arguments($original));

userTarget('discarded');

?>
--EXPECTF--
array(1) {
  [0]=>
  string(266) "message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-%d"
}
partial-created
array(1) {
  [0]=>
  string(266) "message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-%d"
}
array(1) {
  [0]=>
  string(266) "message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-%d"
}
partial-destroyed
array(1) {
  [0]=>
  string(266) "message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-%d"
}

Warning: The return value of function userTarget() should either be used or intentionally ignored by casting it as (void), message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-message-%d in %s on line %d
