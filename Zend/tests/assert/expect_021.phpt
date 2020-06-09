--TEST--
test use of assert_handler callback
--INI--
zend.assertions=1
assert.exception=0
assert.warning=0
assert.bail=0
--FILE--
<?php
class CustomError extends AssertionError {}

assert_options(ASSERT_CALLBACK, "assert_handler");

function assert_handler($file, $line, $assertion, $description="")
{
  echo "file: $file\nline: $line\nassertion: $assertion\ndescription: $description\n\n";
  return true;
}

assert(10 < 5);
assert(3 == 4, 'Assertion Description');
assert(5 * 5 < 25, new CustomError('Assertion Exception'));
?>
--EXPECTF--
file: %sexpect_021.php
line: %d
assertion: 10 < 5
description: assert(10 < 5)

file: %sexpect_021.php
line: %d
assertion: 3 == 4
description: Assertion Description

file: %sexpect_021.php
line: %d
assertion: 5 * 5 < 25
description: CustomError: Assertion Exception in %sexpect_021.php:%d
Stack trace:
#0 {main}