--TEST--
Unique PHP short tag deprecation with multiple short tags used
--FILE--
<?php
 echo "Used a normal tag\n";
?>
<?
 echo "Used a short tag\n";
?>
Non PHP content
<?php
 echo "Another normal tag\n";
?>
<?
 echo "Another short tag\n";
?>
Finished
--EXPECTF--
Deprecated: PHP short tags are deprecated. This message will not be printed for further short tags uses. First usage in %s on line 4
Used a normal tag
Used a short tag
Non PHP content
Another normal tag
Another short tag
Finished
