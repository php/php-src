<?php
/*
 * Common definition and Settings
 */

// Var def for testing
$t_ary = array(
		's1' => '日本語EUC-JPの文字列',
		's2' => 'English Text'
		);

class tc
{
	public $s1 = '日本語EUC-JPの文字列';
	public $s2 = 'English Text';

	function __construct()
	{
	}
}

$t_obj = new tc;

?>
