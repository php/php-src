--TEST--
Use of non-literals in declare ticks values crashes compiler
--FILE--
<?php
declare(ticks = UNKNOWN_CONST) {
  echo 'Done';
}
--EXPECTF--
Fatal error: declare(ticks) value must be a literal in %sdeclare_006.php on line 2
