--TEST--
Testing isset and unset with variable variables
--FILE--
<?php

print "- isset ---\n";

$var_name = 'unexisting';

if (isset($$var_name)) {
    print "error\n";
}

$test = 'var_name';

if (isset($$$test)) {
    print "error\n";
}

print "- unset ---\n";

unset($$var_name);

unset($$$test);

print "done\n";

?>
--EXPECT--
- isset ---
- unset ---
done
