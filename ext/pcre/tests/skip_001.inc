<?php

if (!@setlocale(LC_ALL, 'pt_PT', 'pt', 'pt_PT.ISO8859-1', 'portuguese')) {
	die('skip pt locale not available');
}
