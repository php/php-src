--TEST--
Test parse_url() function : check values of URL related constants
--FILE--
<?php
/*
 *  check values of URL related constants
 */
foreach(get_defined_constants() as $constantName => $constantValue) {
    if (strpos($constantName, 'PHP_URL')===0) {
        echo "$constantName: $constantValue \n";
    }
}

echo "Done";
?>
--EXPECT--
PHP_URL_SCHEME: 0 
PHP_URL_HOST: 1 
PHP_URL_PORT: 2 
PHP_URL_USER: 3 
PHP_URL_PASS: 4 
PHP_URL_PATH: 5 
PHP_URL_QUERY: 6 
PHP_URL_FRAGMENT: 7 
Done
