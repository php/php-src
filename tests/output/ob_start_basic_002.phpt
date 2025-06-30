--TEST--
ob_start(): Check behaviour with various callback return values.
--FILE--
<?php
function return_empty_string($string) {
    return "";
}

function return_false($string) {
    return false;
}

function return_null($string) {
    return null;
}

function return_string($string) {
    return "I stole your output.";
}

function return_zero($string) {
    return 0;
}

// Use each of the above functions as an output buffering callback:
$functions = get_defined_functions();
$callbacks = $functions['user'];
sort($callbacks);
foreach ($callbacks as $callback) {
  echo "--> Use callback '$callback':\n";
  ob_start($callback);
  echo 'My output.';
  ob_end_flush();
  echo "\n\n";
}

?>
--EXPECTF--
--> Use callback 'return_empty_string':


--> Use callback 'return_false':

Deprecated: ob_end_flush(): Returning a non-string result from user output handler return_false is deprecated in %s on line %d
My output.

--> Use callback 'return_null':

Deprecated: ob_end_flush(): Returning a non-string result from user output handler return_null is deprecated in %s on line %d


--> Use callback 'return_string':
I stole your output.

--> Use callback 'return_zero':

Deprecated: ob_end_flush(): Returning a non-string result from user output handler return_zero is deprecated in %s on line %d
0
