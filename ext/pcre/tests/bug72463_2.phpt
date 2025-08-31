--TEST--
Bug #72463 mail fails with invalid argument
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN") {
    die('skip windows only');
}
?>
--INI--
SMTP=non.existent.smtp.server
--FILE--
<?php

mail("some.address.it.wont.ever.reach@lookup.and.try.to.find.this.host.name","subject","a", "");
mail("some.address.it.wont.ever.reach@lookup.and.try.to.find.this.host.name","subject","a", NULL);

?>
===DONE===
--EXPECTREGEX--
.*===DONE===
