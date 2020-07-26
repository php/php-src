<?php

$hash = hash_init('md5');

while (!feof(STDIN)) {
	hash_update($hash, fread(STDIN, 8192));
}

echo hash_final($hash);
