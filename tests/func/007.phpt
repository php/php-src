--TEST--
INI functions test
--FILE--
<?php

$ini1 =  ini_get('include_path'); 
ini_set('include_path','ini_set_works');
echo ini_get('include_path')."\n";
ini_restore('include_path');
$ini2 =  ini_get('include_path'); 

if ($ini1 !== $ini2) {
        echo "ini_restore() does not work.\n";
}
else {
        echo "ini_restore_works\n";
}

?>
--EXPECT--
ini_set_works
ini_restore_works
