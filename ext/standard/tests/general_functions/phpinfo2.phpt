--TEST--
phpinfo() CGI
--POST--
dummy=x
--FILE--
<?php
var_dump(phpinfo());

echo "--\n";
var_dump(phpinfo(0));

echo "--\n";
var_dump(phpinfo(INFO_LICENSE));

?>
--EXPECTF--
<!DOCTYPE %s>
%a</html>bool(true)
--
<!DOCTYPE %s>
%a</html>bool(true)
--
<!DOCTYPE %s>
%a</html>bool(true)
