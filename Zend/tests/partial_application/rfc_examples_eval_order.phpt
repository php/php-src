--TEST--
PFA RFC examples: "Evaluation order" section
--FILE--
<?php

function getArg() {
  print __FUNCTION__ . PHP_EOL;
  return 'hi';
}

function speak(string $who, string $msg) {
  printf("%s: %s\n", $who, $msg);
}

$arrow = static fn($who) => speak($who, getArg());
print "Arnaud\n";
$arrow('Larry');

$partial = speak(?, getArg());
print "Arnaud\n";
$partial('Larry');

?>
--EXPECT--
Arnaud
getArg
Larry: hi
getArg
Arnaud
Larry: hi
