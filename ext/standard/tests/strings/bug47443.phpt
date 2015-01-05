--TEST--
Bug #47443 (metaphone('scratch') returns wrong result)
--FILE--
<?php

var_dump(metaphone("scratch"));
var_dump(metaphone("scrath"));
var_dump(metaphone("scratc"));

?>
--EXPECTF--
%string|unicode%(4) "SKRX"
%string|unicode%(4) "SKR0"
%string|unicode%(5) "SKRTK"
