--TEST--
Bug #72463 mail fails with invalid argument
--FILE--
<?php

mail("some.address.it.wont.ever.reach@lookup.and.try.to.find.this.host.name","subject","a", "");
mail("some.address.it.wont.ever.reach@lookup.and.try.to.find.this.host.name","subject","a", NULL);

?>
===DONE===
--EXPECTF--
%A
===DONE===
