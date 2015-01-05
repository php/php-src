<?php

if (@preg_match_all('/\p{N}/', '0123456789', $dummy) === false) {
	die('skip');
}
