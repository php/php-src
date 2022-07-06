--TEST--
Bug #75556 (Invalid opcode 138/1/1)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
 function createFromFormat($format, $date, ?\DateTimeZone $tz = null): ?\DateTimeInterface
 {
     if ($tz !== null
             || ($tz instanceof \DateTimeZone && !in_array($tz->getName(), ['UTC', 'Z'], true))
        ) {
         $msg = 'Date objects must have UTC as their timezone';
         throw new \UnexpectedValueException($msg);
     }

    return null;
}

var_dump(createFromFormat('m/d/Y', '12/07/2017', null));
?>
--EXPECT--
NULL
