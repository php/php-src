--TEST--
Bug #72463 mail fails with invalid argument
--FILE--
<?php

if(substr(PHP_OS, 0, 3) == "WIN") {
	ini_set("smtp", "non.existent.smtp.server");
} else {
	ini_set("sendmail_path", "echo hello >/dev/null");
}

mail("some.address.it.wont.ever.reach@lookup.and.try.to.find.this.host.name","subject","a", "");
mail("some.address.it.wont.ever.reach@lookup.and.try.to.find.this.host.name","subject","a", NULL);

?>
===DONE===
--EXPECTREGEX--
.*===DONE===
