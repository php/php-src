<?php
return [
	'memory > (2^32)-1 KiB'     => '$argon2id$v=19$m=4294967296,t=1,p=1$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
	'memory uint overflow'      => '$argon2id$v=19$m=92233720368547758070,t=1,p=1$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
	'time > (2^32)-1'           => '$argon2id$v=19$m=8,t=4294967296,p=1$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
	'time uint overflow'        => '$argon2id$v=19$m=8,t=92233720368547758070,p=1$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
	'threads > 255'             => '$argon2id$v=19$m=2048,t=1,p=256$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
	'threads uint overflow'     => '$argon2id$v=19$m=8,t=1,p=92233720368547758070$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
	'memory with -sign'         => '$argon2id$v=19$m=-65536,t=4,p=1$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
	'time with -sign'           => '$argon2id$v=19$m=65536,t=-4,p=1$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
	'threads with -sign'        => '$argon2id$v=19$m=65536,t=4,p=-1$ABCDEFGHIJKLMNOPQRSTUg$VWXYZabcdefghijklmnopqrstuvxyz0123456789+/w',
];
