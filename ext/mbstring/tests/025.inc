<?php
	mb_regex_set_options( 'x' );
	print mb_ereg_replace(' -', '+', '- - - - -' );

	mb_regex_set_options( '' );
	print mb_ereg_replace(' -', '+', '- - - - -' );
?>
