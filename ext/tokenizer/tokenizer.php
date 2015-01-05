<?php

if(!extension_loaded('tokenizer')) {
	    dl('tokenizer.so');
}

$fp = fopen('php://stdin', 'r');
while (!feof($fp)) {
	$content .= fread($fp, 4096);
}
fclose($fp);

$tokens = token_get_all($content);

$count = count($tokens);
$state = 0;
for ($i = 0; $i < $count; $i++) {
	$token = $tokens[$i];
	if (is_array($token)) {
		if ($state == 1 && $token[0] == T_STRING) {
			$token[1] = preg_replace('!([a-z])([A-Z])!e', '"$1_".strtolower("$2")', $token[1]);
			$state = 0;
		} else if ($token[0] == T_FUNCTION) {
			$state = 1;
		}
		$chunk = $token[1];
	} else {
		$chunk = $token;
	}
	$output .= $chunk;
}

print $output;

?>
